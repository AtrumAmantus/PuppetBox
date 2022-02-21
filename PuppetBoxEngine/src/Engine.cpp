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
    namespace Event::Topic
    {
        std::uint32_t NETWORK_TOPIC = 0;
        std::uint32_t ENGINE_ADD_SCENE_TOPIC = 0;
        std::uint32_t ENGINE_SET_SCENE_TOPIC = 0;
    }

    namespace
    {
        std::uint8_t getEndianness()
        {
            std::uint32_t n = 1;
            std::uint8_t* p = (uint8_t*) &n;
            return *p;
        }

        void parseData(std::uint32_t contentLength, std::uint8_t* data, std::uint32_t dataLength)
        {
            std::cout << "Received " << dataLength << " bytes of data" << std::endl;
            std::cout << "Headers:" << std::endl;
            std::cout << "\tMessage Length: " << contentLength << std::endl;
            std::cout << "Raw Packet:" << std::endl << "\t";

            for (std::uint32_t i = 0; i < dataLength; ++i)
            {
                std::cout << data[i] << ", ";
            }

            std::cout << std::endl;
        }

        void networkRunner()
        {
            LOGGER_INFO("Networking thread started.");

            const bool isHostBigEndian = getEndianness() == BIG_ENDIAN;

            struct
            {
                Networking::NetworkingDetails details{};
                bool shouldDisconnect = false;
                bool shouldThreadStop = false;
            } connection;

            Event::Topic::NETWORK_TOPIC = MessageBroker::instance()
                    .subscribe(
                            "pb_network_update",
                            [&connection, isHostBigEndian](std::shared_ptr<void> data) {
                                auto networkEvent = std::static_pointer_cast<NetworkEvent>(data);

                                switch (networkEvent->type)
                                {
                                    case Event::NetworkEventType::CONNECT:
                                        if (!connection.details.isConnected)
                                        {
                                            LOGGER_INFO("Connecting to server " + networkEvent->host + ":" +
                                                        std::to_string(networkEvent->port));
                                            connection.details.isConnected = Networking::connect(
                                                    &(connection.details),
                                                    networkEvent->host,
                                                    networkEvent->port);
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
                                    case Event::NetworkEventType::SEND:
                                        if (connection.details.isConnected)
                                        {
                                            Networking::send(
                                                    &(connection.details),
                                                    networkEvent->data,
                                                    networkEvent->dataLength,
                                                    isHostBigEndian);
                                        }
                                        else
                                        {
                                            LOGGER_ERROR("Not connected to any server");
                                        }

                                        break;
                                    default:
                                        LOGGER_ERROR("Unrecognized network event type");
                                }
                            });

            std::int16_t bytesRead;
            std::uint8_t data[MAX_PACKET_SIZE];
            std::uint8_t buffer[MAX_BUFFER_SIZE];
            std::uint8_t receivedData[MAX_BUFFER_SIZE];
            std::uint32_t bufferOffset = 0;
            std::uint32_t dataInCurrentPacket = 0;

            const std::uint8_t bitShift0 = isHostBigEndian ? 0 : 24;
            const std::uint8_t bitShift1 = isHostBigEndian ? 8 : 16;
            const std::uint8_t bitShift2 = isHostBigEndian ? 16 : 8;
            const std::uint8_t bitShift3 = isHostBigEndian ? 24 : 0;

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
                            if ((bytesRead = Networking::read(&(connection.details), data, MAX_PACKET_SIZE)) > 0)
                            {
                                std::copy(std::begin(data), std::begin(data) + bytesRead,
                                          std::begin(buffer) + bufferOffset);

                                bufferOffset += bytesRead;

                                if (bufferOffset > PACKET_HEADER_SIZE)
                                {
                                    dataInCurrentPacket = (buffer[0] << bitShift0) | (buffer[1] << bitShift1) |
                                                          (buffer[2] << bitShift2) | (buffer[3] << bitShift3);
                                }
                            }
                            else if (bytesRead == -1)
                            {
                                LOGGER_INFO("Lost connection with server");
                                connection.shouldDisconnect = true;
                            }


                            // Check if we read at least as many bytes as we wanted for the current message
                            if (dataInCurrentPacket > 0 && (bufferOffset - PACKET_HEADER_SIZE) >= dataInCurrentPacket)
                            {
                                // Copy message to new array and process
                                std::copy(std::begin(buffer),
                                          std::begin(buffer) + dataInCurrentPacket + PACKET_HEADER_SIZE,
                                          std::begin(receivedData));
                                parseData(dataInCurrentPacket, receivedData, dataInCurrentPacket + PACKET_HEADER_SIZE);

                                // Check if we have leftover bytes from a new message
                                if ((bufferOffset - PACKET_HEADER_SIZE) > dataInCurrentPacket)
                                {
                                    // Get bounds for leftover data in new message
                                    std::uint32_t totalProcessedData = dataInCurrentPacket + PACKET_HEADER_SIZE;
                                    std::uint32_t totalNewData = bufferOffset - totalProcessedData;
                                    std::uint32_t newDataStart = bufferOffset - totalNewData;
                                    // Shift new data down
                                    std::copy(std::begin(buffer) + newDataStart, std::begin(buffer) + bufferOffset,
                                              std::begin(buffer));
                                    // Reset buffer offset
                                    bufferOffset -= totalProcessedData;
                                }
                                else
                                {
                                    // Much easier, just reset everything to 0
                                    dataInCurrentPacket = 0;
                                    bufferOffset = 0;
                                }
                            }
                        }
                    }

                    Networking::close(&(connection.details));
                    connection.details.isConnected = false;
                    connection.shouldDisconnect = false;
                    LOGGER_INFO("Server connection has closed");
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
        currentScene_ = std::make_shared<DefaultSceneGraph>("default", gfxApi_->getRenderWindow(), inputReader_);

        sceneGraphs_.insert(
                std::pair<std::string, std::shared_ptr<AbstractSceneGraph>>{
                        "default",
                        currentScene_
                }
        );

        // Listener for adding new scenes.
        Event::Topic::ENGINE_ADD_SCENE_TOPIC = MessageBroker::instance().subscribe("pb_engine_add_scene", [this](std::shared_ptr<void> data) {
            auto event = std::static_pointer_cast<EngineAddSceneEvent>(data);

            *(event->scene) = AbstractSceneGraph{event->scene->name, gfxApi_->getRenderWindow(), inputReader_};
            event->scene->setUp();

            sceneGraphs_.insert(
                    std::pair<std::string, std::shared_ptr<AbstractSceneGraph>>{event->scene->name, event->scene}
            );
        });

        // Listener for setting the current scene.
        Event::Topic::ENGINE_SET_SCENE_TOPIC = MessageBroker::instance().subscribe("pb_event_set_scene", [this](std::shared_ptr<void> data) {
            auto event = std::static_pointer_cast<EngineSetSceneEvent>(data);

            if (sceneGraphs_.find(event->sceneName) != sceneGraphs_.end())
            {
                currentScene_ = sceneGraphs_.at(event->sceneName);
            }
            else
            {
                LOGGER_ERROR("Unable to locate specified scene: '" + event->sceneName + "'");
            }
        });
    }

    void Engine::run()
    {
        std::thread networkThread{&networkRunner};

        hardwareInitializer_.initializeGameTime();

        while (!inputReader_->window.windowClose)
        {
            float deltaTime = hardwareInitializer_.updateElapsedTime();

            processInput();

            gfxApi_->preLoopCommands();

            currentScene_->update(deltaTime);

            gfxApi_->setTransformUBOData(
                    currentScene_->getView(),
                    currentScene_->getProjection(),
                    currentScene_->getUIProjection());

            currentScene_->render();

            hardwareInitializer_.postLoopCommands();
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