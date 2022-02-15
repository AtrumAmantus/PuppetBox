#include <memory>
#include <string>
#include <utility>

#include <PuppetBox.h>

#include "AnimationCatalogue.h"
#include "AssetLibrary.h"
#include "Engine.h"
#include "FontLoader.h"
#include "OpenGLGfxApi.h"
#include "Sdl2Initializer.h"
#include "Sdl2InputProcessor.h"
#include "UIComponents.h"

namespace PB
{
    namespace
    {
        // Engine context variables
        std::shared_ptr<IHardwareInitializer> hardwareInitializer{nullptr};
        std::shared_ptr<AbstractInputProcessor> inputProcessor{nullptr};
        std::shared_ptr<IGfxApi> gfxApi{nullptr};
        FontLoader fontLoader{nullptr};
        AnimationCatalogue animationCatalogue{nullptr};
        std::unordered_map<std::string, std::shared_ptr<SceneGraph>> loadedScenes{};
        std::shared_ptr<AssetLibrary> assetLibrary{nullptr};
        std::string activeSceneId;
        SceneGraph invalidScene{"InvalidScene", nullptr, nullptr};
        bool pbInitialized = false;

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
        * \brief Helper function that returns a reference to the currently active SceneGraph.
        *
        * \return Reference to the currently active SceneGraph.
        */
        SceneGraph& activeScene()
        {
            if (!activeSceneId.empty() && loadedScenes.find(activeSceneId) != loadedScenes.end())
            {
                return *loadedScenes.at(activeSceneId);
            }

            return invalidScene;
        }

        /**
        * \brief Helper function that converts publicly exposed enum to private one.
        *
        * \param type	The publicly exposed asset type enum.
        *
        * \return The private, inner asset type enum.
        */
        Asset::Type convertToAssetType(LibraryAsset::Type type)
        {
            switch (type)
            {
                case LibraryAsset::Type::MODEL_2D:
                    return Asset::Type::MODEL_2D;
                default:
                    return Asset::Type::UNKNOWN;
            }
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

    void Init(const std::string& windowTitle, std::int32_t windowWidth, std::int32_t windowHeight, std::int32_t renderDepth)
    {
        Init_CharMap();

        std::shared_ptr<IGfxApi> gfxApi_ = defaultGfxApi();

        // Create hardware Api instance
        std::shared_ptr<Sdl2InputProcessor> inputProcessor_ = std::make_shared<Sdl2InputProcessor>();
        std::shared_ptr<Sdl2Initializer> hardwareInitializer_ = std::make_shared<Sdl2Initializer>(*gfxApi_);

#ifdef _DEBUG
        hardwareInitializer_->enableDebugger();
#endif

        if (hardwareInitializer_->init(windowTitle, windowWidth, windowHeight, renderDepth))
        {
            std::cout << hardwareInitializer_->initializerName() << " loaded." << std::endl;

            gfxApi = gfxApi_;
            hardwareInitializer = hardwareInitializer_;
            inputProcessor = inputProcessor_;

            pbInitialized = true;

            bool error = false;
            //TODO: Using globally instanced font loader?
            assetLibrary = std::make_shared<AssetLibrary>("../", gfxApi, &fontLoader);
            animationCatalogue = AnimationCatalogue(assetLibrary);
            LoadAssetPack("Assets1");
            //TODO: This needs to go somewhere else, or otherwise not hardcoded with a path.
            //TODO: Perhaps build a default path for default assets
            Shader glyphShader = assetLibrary->loadShaderAsset("Assets1/Shaders/UI/Glyph", &error);

            if (error)
            {
                LOGGER_ERROR("Failed to load glyph shader");
            }
            else
            {
                LOGGER_DEBUG("Glyph shader loaded");
                fontLoader = FontLoader{gfxApi_};
                LOGGER_DEBUG("FontLoader initialized");
            }
        }
        else
        {
            LOGGER_ERROR("Failed to initialize hardware");
        }
    }

    void CreateScene(const std::string& sceneName)
    {
        if (loadedScenes.find(sceneName) == loadedScenes.end())
        {
            std::shared_ptr<SceneGraph> scene = std::make_shared<SceneGraph>(sceneName, &(*gfxApi), &(*inputProcessor));

            loadedScenes.insert(
                    std::pair<std::string, std::shared_ptr<SceneGraph>>{sceneName, scene}
            );
        }
        else
        {
            LOGGER_ERROR("Scene already exists with this identifier name");
        }
    }

    void SetActiveScene(const std::string& sceneName)
    {
        if (loadedScenes.find(sceneName) != loadedScenes.end())
        {
            activeSceneId = sceneName;
        }
        else
        {
            LOGGER_ERROR("Scene does not exist with this identifier name");
        }
    }

    void SetSceneHandler(AbstractSceneHandler* sceneHandler)
    {
        activeScene().setSceneHandler(sceneHandler);
    }

    bool LoadAssetPack(const std::string& archiveName)
    {
        return assetLibrary->loadArchive(archiveName);
    }

    bool LoadFontAsset(const std::string& fontPath, std::uint8_t fontSize)
    {
        bool error = false;
        assetLibrary->loadFontAsset(fontPath, fontSize, &error);
        return !error;
    }

    bool CreateSceneObject(const std::string& assetPath, SceneObject* sceneObject, LibraryAsset::Type type)
    {
        bool success;

        if (sceneObject == nullptr)
        {
            success = false;
            LOGGER_ERROR("SceneObject must be instantiated prior to invoking CreateSceneObject");
        }
        else
        {
            success = assetLibrary->loadModelAsset(assetPath, sceneObject, convertToAssetType(type));
            sceneObject->setAnimationCatalogue(&animationCatalogue);
        }

        return success;
    }

    bool LoadAnimationsPack(const std::string& assetPath)
    {
        return animationCatalogue.load(assetPath);
    }

    void Run()
    {
        if (pbInitialized)
        {
            Engine engine{*gfxApi, *hardwareInitializer, *inputProcessor};

            engine.setScene(&activeScene());

            engine.run();

            engine.shutdown();
        }
        else
        {
            LOGGER_ERROR("PuppetBox context was not initialized, must call PB::Init() before PB::Run()");
            hardwareInitializer->destroy();
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
                LOGGER_ERROR("Unrecognized component type: " + uiComponentType);
        }

        *error = component == nullptr || !component->init();

        if (*error)
        {
            LOGGER_ERROR("Failed to load UI Component");
        }

        return component;
    }
}