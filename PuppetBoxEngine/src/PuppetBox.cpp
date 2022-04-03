#include <utility>
#include <unordered_map>

#include <PuppetBox.h>
#include "puppetbox/KeyCode.h"

#include "AnimationCatalogue.h"
#include "AssetLibrary.h"
#include "Engine.h"
#include "EventDef.h"
#include "FontLoader.h"
#include "MessageBroker.h"
#include "OpenGLGfxApi.h"
#include "Sdl2Initializer.h"
#include "Sdl2InputReader.h"
#include "UIComponents.h"

namespace PB
{
    namespace
    {
        // Engine context variables
        Sdl2Initializer hardwareInitializer{nullptr};
        std::shared_ptr<AbstractInputReader> inputReader{nullptr};
        std::shared_ptr<IGfxApi> gfxApi{nullptr};
        FontLoader fontLoader{nullptr};
        AnimationCatalogue animationCatalogue{nullptr};
        std::shared_ptr<AssetLibrary> assetLibrary{nullptr};
        bool pbInitialized = false;
        bool engineInitialized = false;

        /**
        * \brief Used to map scan codes to the actual ascii characters
        * Key: scancode, Value: character
        */
        std::unordered_map<std::uint8_t, std::int8_t> charMap_{};
        /**
        * \brief Used to map scan codes to the actual ascii characters if shift was held
        * Key: scancode, Value: character
        */
        std::unordered_map<std::uint8_t, std::int8_t> shiftedCharMap_{};

        /**
        * \brief Helper method to insider values into the charMap.
        *
        * \param k	The scancode for the entry.
        * \param v	The value for the entry.
        */
        void CharMapInsert(std::uint8_t k, std::uint8_t v, bool shifted = false)
        {
            if (shifted)
            {
                shiftedCharMap_.insert(std::pair<std::uint8_t, std::uint8_t>{k, v});
            }
            else
            {
                charMap_.insert(std::pair<std::uint8_t, std::uint8_t>{k, v});
            }
        }

        /**
        * \brief Helper function that provides a default IGfxApi implementation.
        *
        * \return A IGfxApi implementation.
        */
        std::shared_ptr<IGfxApi> defaultGfxApi()
        {
            return std::make_shared<OpenGLGfxApi>();
        }

        /**
        * \brief Initializes the char map with the desired mappings of arbitrary key codes to specific ascii characters.
        */
        void Init_CharMap()
        {
            CharMapInsert(KEY_1, '1');
            CharMapInsert(KEY_2, '2');
            CharMapInsert(KEY_3, '3');
            CharMapInsert(KEY_4, '4');
            CharMapInsert(KEY_5, '5');
            CharMapInsert(KEY_6, '6');
            CharMapInsert(KEY_7, '7');
            CharMapInsert(KEY_8, '8');
            CharMapInsert(KEY_9, '9');
            CharMapInsert(KEY_0, '0');

            CharMapInsert(KEY_A, 'a');
            CharMapInsert(KEY_B, 'b');
            CharMapInsert(KEY_C, 'c');
            CharMapInsert(KEY_D, 'd');
            CharMapInsert(KEY_E, 'e');
            CharMapInsert(KEY_F, 'f');
            CharMapInsert(KEY_G, 'g');
            CharMapInsert(KEY_H, 'h');
            CharMapInsert(KEY_I, 'i');
            CharMapInsert(KEY_J, 'j');
            CharMapInsert(KEY_K, 'k');
            CharMapInsert(KEY_L, 'l');
            CharMapInsert(KEY_M, 'm');
            CharMapInsert(KEY_N, 'n');
            CharMapInsert(KEY_O, 'o');
            CharMapInsert(KEY_P, 'p');
            CharMapInsert(KEY_Q, 'q');
            CharMapInsert(KEY_R, 'r');
            CharMapInsert(KEY_S, 's');
            CharMapInsert(KEY_T, 't');
            CharMapInsert(KEY_U, 'u');
            CharMapInsert(KEY_V, 'v');
            CharMapInsert(KEY_W, 'w');
            CharMapInsert(KEY_X, 'x');
            CharMapInsert(KEY_Y, 'y');
            CharMapInsert(KEY_Z, 'z');

            CharMapInsert(KEY_SPACE, ' ');
            CharMapInsert(KEY_MINUS, '-');
            CharMapInsert(KEY_EQUAL, '=');
            CharMapInsert(KEY_LEFT_BRACKET, '[');
            CharMapInsert(KEY_RIGHT_BRACKET, ']');
            CharMapInsert(KEY_BACKSLASH, '\\');
            CharMapInsert(KEY_SEMICOLON, ';');
            CharMapInsert(KEY_APOSTROPHE, '\'');
            CharMapInsert(KEY_GRAVE_ACCENT, '`');
            CharMapInsert(KEY_COMMA, ',');
            CharMapInsert(KEY_PERIOD, '.');
            CharMapInsert(KEY_SLASH, '/');

            CharMapInsert(KEY_KP_DIVIDE, '/');
            CharMapInsert(KEY_KP_MULTIPLY, '*');
            CharMapInsert(KEY_KP_SUBTRACT, '-');
            CharMapInsert(KEY_KP_ADD, '+');
            CharMapInsert(KEY_KP_1, '1');
            CharMapInsert(KEY_KP_2, '2');
            CharMapInsert(KEY_KP_3, '3');
            CharMapInsert(KEY_KP_4, '4');
            CharMapInsert(KEY_KP_5, '5');
            CharMapInsert(KEY_KP_6, '6');
            CharMapInsert(KEY_KP_7, '7');
            CharMapInsert(KEY_KP_8, '8');
            CharMapInsert(KEY_KP_9, '9');
            CharMapInsert(KEY_KP_0, '0');
            CharMapInsert(KEY_KP_DECIMAL, '.');

            CharMapInsert(KEY_1, '!', true);
            CharMapInsert(KEY_2, '@', true);
            CharMapInsert(KEY_3, '#', true);
            CharMapInsert(KEY_4, '$', true);
            CharMapInsert(KEY_5, '%', true);
            CharMapInsert(KEY_6, '^', true);
            CharMapInsert(KEY_7, '&', true);
            CharMapInsert(KEY_8, '*', true);
            CharMapInsert(KEY_9, '(', true);
            CharMapInsert(KEY_0, ')', true);

            CharMapInsert(KEY_A, 'A', true);
            CharMapInsert(KEY_B, 'B', true);
            CharMapInsert(KEY_C, 'C', true);
            CharMapInsert(KEY_D, 'D', true);
            CharMapInsert(KEY_E, 'E', true);
            CharMapInsert(KEY_F, 'F', true);
            CharMapInsert(KEY_G, 'G', true);
            CharMapInsert(KEY_H, 'H', true);
            CharMapInsert(KEY_I, 'I', true);
            CharMapInsert(KEY_J, 'J', true);
            CharMapInsert(KEY_K, 'K', true);
            CharMapInsert(KEY_L, 'L', true);
            CharMapInsert(KEY_M, 'M', true);
            CharMapInsert(KEY_N, 'N', true);
            CharMapInsert(KEY_O, 'O', true);
            CharMapInsert(KEY_P, 'P', true);
            CharMapInsert(KEY_Q, 'Q', true);
            CharMapInsert(KEY_R, 'R', true);
            CharMapInsert(KEY_S, 'S', true);
            CharMapInsert(KEY_T, 'T', true);
            CharMapInsert(KEY_U, 'U', true);
            CharMapInsert(KEY_V, 'V', true);
            CharMapInsert(KEY_W, 'W', true);
            CharMapInsert(KEY_X, 'X', true);
            CharMapInsert(KEY_Y, 'Y', true);
            CharMapInsert(KEY_Z, 'Z', true);

            CharMapInsert(KEY_SPACE, ' ', true);
            CharMapInsert(KEY_MINUS, '_', true);
            CharMapInsert(KEY_EQUAL, '+', true);
            CharMapInsert(KEY_LEFT_BRACKET, '{', true);
            CharMapInsert(KEY_RIGHT_BRACKET, '}', true);
            CharMapInsert(KEY_BACKSLASH, '|', true);
            CharMapInsert(KEY_SEMICOLON, ':', true);
            CharMapInsert(KEY_APOSTROPHE, '"', true);
            CharMapInsert(KEY_GRAVE_ACCENT, '~', true);
            CharMapInsert(KEY_COMMA, '<', true);
            CharMapInsert(KEY_PERIOD, '>', true);
            CharMapInsert(KEY_SLASH, '?', true);

            CharMapInsert(KEY_KP_DIVIDE, '/', true);
            CharMapInsert(KEY_KP_MULTIPLY, '*', true);
            CharMapInsert(KEY_KP_SUBTRACT, '-', true);
            CharMapInsert(KEY_KP_ADD, '+', true);
            CharMapInsert(KEY_KP_1, '1', true);
            CharMapInsert(KEY_KP_2, '2', true);
            CharMapInsert(KEY_KP_3, '3', true);
            CharMapInsert(KEY_KP_4, '4', true);
            CharMapInsert(KEY_KP_5, '5', true);
            CharMapInsert(KEY_KP_6, '6', true);
            CharMapInsert(KEY_KP_7, '7', true);
            CharMapInsert(KEY_KP_8, '8', true);
            CharMapInsert(KEY_KP_9, '9', true);
            CharMapInsert(KEY_KP_0, '0', true);
            CharMapInsert(KEY_KP_DECIMAL, '.', true);
        }
    }

    void Init(
            const std::string& windowTitle,
            std::int32_t windowWidth,
            std::int32_t windowHeight,
            std::int32_t renderDepth)
    {
        Init_CharMap();

        // Initialize APIs
        gfxApi = defaultGfxApi();
        hardwareInitializer = Sdl2Initializer{gfxApi};
        inputReader = std::make_shared<Sdl2InputReader>();

#ifdef _DEBUG
        hardwareInitializer.enableDebugger();
#endif

        if (hardwareInitializer.init(windowTitle, windowWidth, windowHeight, renderDepth))
        {
            std::cout << hardwareInitializer.initializerName() << " loaded." << std::endl;

            pbInitialized = true;

            //TODO: Using globally instanced font loader?
            assetLibrary = std::make_shared<AssetLibrary>("../", gfxApi, &fontLoader);

            if (assetLibrary->init())
            {
                animationCatalogue = AnimationCatalogue(assetLibrary);

                fontLoader = FontLoader{gfxApi};
                LOGGER_DEBUG("FontLoader initialized");
            }
        }
        else
        {
            LOGGER_ERROR("Failed to initialize hardware");
        }
    }

    void CreateScene(std::shared_ptr<AbstractSceneGraph> scene)
    {
        if (engineInitialized)
        {
            auto event = std::make_shared<EngineAddSceneEvent>();
            event->scene = scene;
            MessageBroker::instance().publish(Event::Topic::ENGINE_ADD_SCENE_TOPIC, event);
        }
        else
        {
            LOGGER_ERROR("A scene can't be created until after the engine is running");
        }
    }

    void SetActiveScene(const std::string& sceneName)
    {
        if (engineInitialized)
        {
            auto event = std::make_shared<EngineSetSceneEvent>();
            event->sceneName = sceneName;
            event->resetLast = true;
            MessageBroker::instance().publish(Event::Topic::ENGINE_SET_SCENE_TOPIC, event);
        }
        else
        {
            LOGGER_ERROR("A scene can't be set until after the engine is running");
        }
    }

    bool LoadAssetPack(const std::string& archiveName)
    {
        return assetLibrary->loadArchive(archiveName);
    }

    std::uint32_t GetMeshAsset(const std::string& assetPath, bool* error)
    {
        return assetLibrary->loadMeshAsset(assetPath, error);
    }

    std::uint32_t GetImageMapAsset(const std::string& assetPath, bool* error)
    {
        return assetLibrary->loadImageMapAsset(assetPath, error);
    }

    std::uint32_t GetShaderAsset(const std::string& assetPath, bool* error)
    {
        return assetLibrary->loadShaderAsset(assetPath, error);
    }

    BoneMap GetSkeletonAsset(const std::string& assetPath, bool* error)
    {
        return std::move(assetLibrary->loadSkeletonAsset(assetPath, error));
    }

    bool LoadFontAsset(const std::string& fontPath, std::uint8_t fontSize)
    {
        bool error = false;
        assetLibrary->loadFontAsset(fontPath, fontSize, &error);
        return !error;
    }

    bool LoadAnimationsPack(const std::string& assetPath)
    {
        return animationCatalogue.load(assetPath);
    }

    bool PreloadAnimationFrames(const std::string& animationPath, BoneMap& boneMap)
    {
        return animationCatalogue.preloadAnimation(boneMap, animationPath);
    }

    void Run(std::function<bool()> onReady)
    {
        if (pbInitialized)
        {
            Engine engine{gfxApi, hardwareInitializer, inputReader};

            engine.init();

            engineInitialized = true;

            engine.run(onReady);

            engine.shutdown();
        }
        else
        {
            LOGGER_ERROR("PuppetBox context was not initialized, must call PB::Init() before PB::Run()");
            hardwareInitializer.destroy();
        }
    }

    std::int8_t GetCharFromCode(std::uint8_t code, bool shifted)
    {
        if (shifted)
        {
            if (shiftedCharMap_.find(code) != shiftedCharMap_.end())
            {
                return shiftedCharMap_.at(code);
            }
        }
        else
        {
            if (charMap_.find(code) != charMap_.end())
            {
                return charMap_.at(code);
            }
        }

        return '\0';
    }

    //TODO: Does sending the UIComponentAttributes make sense if they can just be added after via the object?
    UIComponent*
    CreateUIComponent(UI::Type uiComponentType, std::unique_ptr<UIComponentAttributes> attributes, bool* error)
    {
        UIComponent* component{nullptr};

        switch (uiComponentType)
        {
            case UI::TEXT_AREA:
                component = new TextAreaComponent(assetLibrary, gfxApi);
                component->setAttributes(std::move(attributes));
                break;
            case UI::GROUP:
                component = new GroupComponent(assetLibrary, gfxApi);
                component->setAttributes(std::move(attributes));
                break;
            default:
                LOGGER_ERROR("Unrecognized component type: " + std::to_string(uiComponentType));
        }

        *error = component == nullptr || !component->init();

        if (*error)
        {
            LOGGER_ERROR("Failed to load UI Component");
        }

        return component;
    }

    std::uint32_t PublishEvent(const std::string& topicName, std::shared_ptr<void> data)
    {
        return MessageBroker::instance().publish(topicName, data);
    }

    void PublishEvent(std::uint32_t topicId, std::shared_ptr<void> data)
    {
        MessageBroker::instance().publish(topicId, data);
    }

    UUID SubscribeEvent(const std::string& topicName, std::function<void(std::shared_ptr<void>)> callback)
    {
        return MessageBroker::instance().subscribe(topicName, callback);
    }

    void Unsubscribe(UUID uuid)
    {
        MessageBroker::instance().unsubscribe(uuid);
    }

    std::uint32_t RegisterTopic(const std::string& topicName)
    {
        return MessageBroker::instance().registerTopic(topicName);
    }

    void RegisterNetworkEventWriter(const std::string& topicName, pb_NetworkEventWriter writer)
    {
        auto listenerEvent = std::make_shared<NetworkEventWriterEvent>();
        listenerEvent->topicName = topicName;
        listenerEvent->writer = writer;
        MessageBroker::instance().publish(Event::Topic::NETWORK_EVENT_WRITER_TOPIC, listenerEvent);
    }

    void RegisterNetworkEventReader(pb_NetworkEventReader reader)
    {
        auto readerEvent = std::make_shared<NetworkEventReaderEvent>();
        readerEvent->reader = reader;
        MessageBroker::instance().publish(Event::Topic::NETWORK_EVENT_READER_TOPIC, readerEvent);
    }

    void ClearNetworkEventReader()
    {
        auto readerEvent = std::make_shared<NetworkEventReaderEvent>();
        readerEvent->reader = nullptr;
        MessageBroker::instance().publish(Event::Topic::NETWORK_EVENT_READER_TOPIC, readerEvent);
    }
}