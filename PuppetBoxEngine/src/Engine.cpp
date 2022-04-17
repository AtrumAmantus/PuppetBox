#include "DefaultSceneGraph.h"
#include "Engine.h"
#include "EventDef.h"
#include "MessageBroker.h"
#include "Networking.h"

#define MAX_BUFFER_SIZE 0xffff
#define MAX_PACKET_SIZE 0xff
#define PACKET_HEADER_SIZE 0x4 // 4 byte header defines length

#define BIG_ENDIAN 0

namespace PB
{
    namespace
    {
        void defaultReader(std::uint8_t* data, std::uint32_t dataLength)
        {

        }

        inline std::uint8_t getEndianness()
        {
            std::uint32_t n = 1;
            std::uint8_t* p = (uint8_t*) &n;
            return *p;
        }

        inline void parseData(
                pb_NetworkEventReader reader,
                std::uint8_t* data,
                std::uint32_t dataLength)
        {
            reader(data, dataLength);
        }

        void networkRunner()
        {
            LOGGER_INFO("Networking thread started.");

            const bool isHostBigEndian = getEndianness() == BIG_ENDIAN;

            pb_NetworkEventReader networkReader = &defaultReader;

            struct
            {
                Networking::NetworkingDetails details{};
                bool shouldDisconnect = false;
                bool shouldThreadStop = false;
            } connection;

            // Subscribe to listener events
            MessageBroker::instance().subscribe(
                    PB_EVENT_NETWORK_WRITER,
                    [&connection, isHostBigEndian](std::shared_ptr<void> data) {
                        auto listenerEvent = std::static_pointer_cast<NetworkEventWriterEvent>(data);
                        auto transformer = listenerEvent->writer;

                        MessageBroker::instance().subscribe(
                                listenerEvent->topicName,
                                [&connection, transformer, isHostBigEndian](std::shared_ptr<void> d) {
                                    if (connection.details.isConnected)
                                    {
                                        std::uint8_t* byteData = nullptr;
                                        std::uint32_t dataLength;
                                        transformer(d, &byteData, &dataLength);

                                        Networking::send(
                                                &(connection.details),
                                                byteData,
                                                dataLength,
                                                isHostBigEndian);

                                        delete[] byteData;
                                    }
                                });
                    });

            MessageBroker::instance().subscribe(
                    PB_EVENT_NETWORK_READER,
                    [&networkReader](std::shared_ptr<void> data) {
                        auto networkReaderEvent = std::static_pointer_cast<NetworkEventReaderEvent>(data);

                        networkReader =
                                networkReaderEvent->reader != nullptr ? networkReaderEvent->reader : &defaultReader;
                    });

            // Subscribe to network events
            MessageBroker::instance().subscribe(
                    PB_EVENT_NETWORK,
                    [&connection, isHostBigEndian](std::shared_ptr<void> data) {
                        auto networkEvent = std::static_pointer_cast<NetworkEvent>(data);

                        switch (networkEvent->type)
                        {
                            case Event::NetworkEventType::READY_CHECK:
                            {
                                auto networkReadyEvent = std::make_shared<NetworkStatusEvent>();
                                networkReadyEvent->status = Event::NetworkStatus::READY;
                                MessageBroker::instance().publish(
                                        Event::Topic::NETWORK_STATUS_TOPIC,
                                        networkReadyEvent);
                            }
                                break;
                            case Event::NetworkEventType::CONNECT:
                                if (!connection.details.isConnected)
                                {
                                    LOGGER_INFO("Connecting to server " + networkEvent->host + ":" +
                                                std::to_string(networkEvent->port));
                                    connection.shouldDisconnect = false;
                                    connection.details.isConnected = Networking::connect(
                                            &(connection.details),
                                            networkEvent->host,
                                            networkEvent->port);

                                    auto event = std::make_shared<NetworkStatusEvent>();
                                    event->status = connection.details.isConnected ? Event::CONNECTED
                                                                                   : Event::DISCONNECTED;
                                    MessageBroker::instance().publish(Event::Topic::NETWORK_STATUS_TOPIC,
                                                                      event);
                                }
                                else
                                {
                                    LOGGER_WARN("Already connected to a server");
                                }

                                break;
                            case Event::NetworkEventType::TERMINATE:
                                connection.shouldThreadStop = true;
                            case Event::NetworkEventType::DISCONNECT:
                                connection.shouldDisconnect = true;
                                LOGGER_INFO("Disconnect request received");
                                break;
                            default:
                                LOGGER_ERROR("Unrecognized network event type");
                        }
                    });

            std::int16_t bytesRead;
            std::uint8_t packetData[MAX_PACKET_SIZE];
            std::uint8_t bufferedData[MAX_BUFFER_SIZE];
            std::uint32_t messageStartIndex = 0;
            std::uint32_t bufferedDataOffset = 0;
            std::uint32_t currentEventDataLength = 0;

            const std::uint8_t bitShift0 = isHostBigEndian ? 0 : 24;
            const std::uint8_t bitShift1 = isHostBigEndian ? 8 : 16;
            const std::uint8_t bitShift2 = isHostBigEndian ? 16 : 8;
            const std::uint8_t bitShift3 = isHostBigEndian ? 24 : 0;

            auto networkReadyEvent = std::make_shared<NetworkStatusEvent>();
            networkReadyEvent->status = Event::NetworkStatus::READY;
            MessageBroker::instance().publish(Event::Topic::NETWORK_STATUS_TOPIC, networkReadyEvent);

            while (!connection.shouldThreadStop)
            {
                // TODO: Maybe suspend the thread until we connect again?
                if (connection.details.isConnected)
                {
                    while (!connection.shouldDisconnect)
                    {
                        // Marks sockets as ready for later read checks
                        std::uint32_t socketsReady = SDLNet_CheckSockets(connection.details.socket_set, 1000);

                        if (socketsReady > 0)
                        {
                            // Read from socket
                            if ((bytesRead = Networking::read(&(connection.details), packetData, MAX_PACKET_SIZE)) > 0)
                            {
                                LOGGER_DEBUG("Received packet, size: " + std::to_string(bytesRead));

                                std::copy(std::begin(packetData),
                                          std::begin(packetData) + bytesRead,
                                          std::begin(bufferedData) + bufferedDataOffset);

                                messageStartIndex = 0;
                                bufferedDataOffset += bytesRead;

                                if (bufferedDataOffset > PACKET_HEADER_SIZE)
                                {
                                    currentEventDataLength = (bufferedData[messageStartIndex + 0] << bitShift0)
                                                             | (bufferedData[messageStartIndex + 1] << bitShift1)
                                                             | (bufferedData[messageStartIndex + 2] << bitShift2)
                                                             | (bufferedData[messageStartIndex + 3] << bitShift3);
                                }

                                // As long as we have complete events to parse
                                while (currentEventDataLength > 0 &&
                                       (bufferedDataOffset - messageStartIndex - PACKET_HEADER_SIZE) >=
                                       currentEventDataLength)
                                {

                                    parseData(
                                            networkReader,
                                            &bufferedData[messageStartIndex],
                                            currentEventDataLength + PACKET_HEADER_SIZE);

                                    // Jump to next event (if there is one)
                                    messageStartIndex += (currentEventDataLength + PACKET_HEADER_SIZE);

                                    // Check if we have leftover bytes from a new message
                                    if (bufferedDataOffset > messageStartIndex)
                                    {
                                        // Get the next event packetData length if we have the bytes for it
                                        if (bufferedDataOffset > (messageStartIndex + PACKET_HEADER_SIZE))
                                        {
                                            currentEventDataLength = (bufferedData[messageStartIndex + 0] << bitShift0)
                                                                     | (bufferedData[messageStartIndex + 1] << bitShift1)
                                                                     | (bufferedData[messageStartIndex + 2] << bitShift2)
                                                                     | (bufferedData[messageStartIndex + 3] << bitShift3);
                                        }
                                        else
                                        {
                                            // Shift new packetData down
                                            std::copy(
                                                    std::begin(bufferedData) + messageStartIndex,
                                                    std::begin(bufferedData) + bufferedDataOffset,
                                                    std::begin(bufferedData));

                                            bufferedDataOffset -= messageStartIndex;
                                            messageStartIndex = 0;

                                            // Reset to 0 to break the loop
                                            currentEventDataLength = 0;
                                        }
                                    }
                                    else
                                    {
                                        // Much easier, just reset everything to 0
                                        messageStartIndex = 0;
                                        currentEventDataLength = 0;
                                        bufferedDataOffset = 0;
                                    }
                                }
                            }
                            else if (bytesRead == -1)
                            {
                                LOGGER_INFO("Lost connection with server");
                                connection.shouldDisconnect = true;
                            }
                        }
                    }

                    if (connection.shouldDisconnect)
                    {
                        LOGGER_INFO("Connection closed by client request");
                    }

                    Networking::close(&(connection.details));
                    connection.details.isConnected = false;
                    connection.shouldDisconnect = false;
                    LOGGER_INFO("Server connection has closed");

                    auto event = std::make_shared<NetworkStatusEvent>();
                    event->status = Event::DISCONNECTED;
                    MessageBroker::instance().publish(Event::Topic::NETWORK_STATUS_TOPIC, event);
                }
            }

            LOGGER_INFO("Networking thread is ready to close");
        }
    }

    Engine::Engine(
            std::shared_ptr<IGfxApi>& gfxApi,
            Sdl2Initializer hardwareInitializer,
            std::shared_ptr<AbstractInputReader>& inputReader)
            : gfxApi_(gfxApi), hardwareInitializer_(std::move(hardwareInitializer)), inputReader_(inputReader)
    {

    }

    void Engine::init()
    {
        currentScene_ = std::make_shared<DefaultSceneGraph>(
                "default",
                gfxApi_->getRenderWindow(),
                inputReader_,
                gfxApi_->createRenderComponent());

        currentScene_->setUp();

        sceneGraphs_.insert(
                std::pair<std::string, std::shared_ptr<AbstractSceneGraph>>{
                        "default",
                        currentScene_
                }
        );

        // Listener for adding new scenes.
        MessageBroker::instance().subscribe(
                PB_EVENT_SCENE_ADD,
                [this](std::shared_ptr<void> data) {
                    auto event = std::static_pointer_cast<EngineAddSceneEvent>(data);

                    // Re-initialize base class properties to required values
                    *(event->scene) = AbstractSceneGraph{
                            event->scene->name,
                            gfxApi_->getRenderWindow(),
                            inputReader_,
                            gfxApi_->createRenderComponent()};

                    sceneGraphs_.insert(
                            std::pair<std::string, std::shared_ptr<AbstractSceneGraph>>{
                                    event->scene->name,
                                    event->scene}
                    );
                });

        // Listener for setting the current scene.
        MessageBroker::instance().subscribe(
                PB_EVENT_SCENE_SET,
                [this](std::shared_ptr<void> data) {
                    auto event = std::static_pointer_cast<EngineSetSceneEvent>(data);

                    if (sceneGraphs_.find(event->sceneName) != sceneGraphs_.end())
                    {
                        nextScene_ = sceneGraphs_.at(event->sceneName);
                        resetScene_ = event->resetLast;
                    }
                    else
                    {
                        LOGGER_ERROR("Unable to locate specified scene: '" + event->sceneName + "'");
                    }
                });
    }

    void Engine::run(std::function<bool()> onReady)
    {
        std::thread networkThread{&networkRunner};

        if (onReady())
        {
            hardwareInitializer_.initializeGameTime();

            while (!inputReader_->window.windowClose)
            {
                float deltaTime = hardwareInitializer_.updateElapsedTime();

                if (nextScene_ != nullptr)
                {
                    if (resetScene_ && currentScene_ != nullptr)
                    {
                        currentScene_->tearDown();
                    }

                    currentScene_ = nextScene_;
                    if (!currentScene_->setUp())
                    {
                        LOGGER_ERROR("Failed to set up scene");
                        inputReader_->window.windowClose = true;
                    }
                    nextScene_ = nullptr;
                }

                processInput();

                gfxApi_->preLoopCommands();

                currentScene_->update(deltaTime);

                // Set common transforms for all shaders
                gfxApi_->setTransformUBOData(
                        currentScene_->getView(),
                        currentScene_->getProjection(),
                        currentScene_->getUIProjection());

                currentScene_->render();

                hardwareInitializer_.postLoopCommands();
            }

            currentScene_->tearDown();
        }
        else
        {
            LOGGER_ERROR("Invocation of ready state logic failed.");
        }

        // Send message to terminate network thread so join() can be called
        auto event = std::make_shared<NetworkEvent>();
        event->type = Event::NetworkEventType::TERMINATE;

        MessageBroker::instance().publish(Event::Topic::NETWORK_TOPIC, event);

        networkThread.join();
    }

    void Engine::shutdown()
    {
        hardwareInitializer_.destroy();
    }

    void Engine::processInput()
    {
        inputReader_->loadCurrentState();

        if (inputReader_->window.newWidth != 0 || inputReader_->window.newHeight != 0)
        {
            gfxApi_->setRenderDimensions(inputReader_->window.newWidth, inputReader_->window.newHeight);
        }

        currentScene_->processInput();
    }
}