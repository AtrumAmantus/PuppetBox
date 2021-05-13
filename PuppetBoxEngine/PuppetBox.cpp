#include "pch.h"

#include <memory>
#include <string>

#include <PuppetBox.h>
#include <STBI/stb_image.h>

#include "../include/puppetbox/AbstractInputProcessor.h"
#include "../include/puppetbox/KeyCode.h"
#include "AssetLibrary.h"
#include "Engine.h"
#include "OpenGLGfxApi.h"
#include "IGfxApi.h"
#include "IHardwareInitializer.h"
#include "SceneGraph.h"
#include "Sdl2Initializer.h"
#include "Sdl2InputProcessor.h"

namespace PB
{
	namespace
	{
		// Engine context variables
		std::shared_ptr<IHardwareInitializer> hardwareInitializer{ nullptr };
		std::shared_ptr<AbstractInputProcessor> inputProcessor{ nullptr };
		std::shared_ptr<IGfxApi> gfxApi{ nullptr };
		std::unordered_map<std::string, std::shared_ptr<SceneGraph>> loadedScenes{};
		std::unique_ptr<AssetLibrary> assetLibrary{ nullptr };
		std::string activeSceneId;
		SceneGraph invalidScene{ "InvalidScene", nullptr, nullptr };
		bool pbInitialized = false;
		/**
		* \brief Used to map scan codes to actual ascii characters
		* Key: scancode, Value: character
		*/
		std::unordered_map<uint8_t, uint8_t> charMap_{};

		/**
		* \brief Helper method to insider values into the charMap.
		*
		* \param k	The scancode for the entry.
		* \param v	The value for the entry.
		*/
		void CharMapInsert(uint8_t k, uint8_t v)
		{
			charMap_.insert(std::pair<uint8_t, uint8_t>{ k, v });
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
				break;
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
			CharMapInsert(KEY_X, 'y');
			CharMapInsert(KEY_Y, 'x');
			CharMapInsert(KEY_Z, 'z');
		}

		/**
		* \brief Helper function for getting the ascii character the given key code represents.
		*
		* \param code	The arbitrary key code to be maped to an ascii character.
		*
		* \return The int value for the ascii character the key code represents.
		*/
		int8_t GetCharFromCode(uint8_t code)
		{
			if (charMap_.find(code) != charMap_.end())
			{
				return charMap_.at(code);
			}

			return 0;
		}
	}

	void Init(std::string windowTitle, uint32_t windowWidth, uint32_t windowHeight)
	{
		Init_CharMap();

		// Create hardware Api instance
		std::shared_ptr<Sdl2InputProcessor> inputProcessor_ = std::make_shared<Sdl2InputProcessor>();
		std::shared_ptr<Sdl2Initializer> hardwareInitializer_ = std::make_shared<Sdl2Initializer>();

		hardwareInitializer_->init(windowTitle, windowWidth, windowHeight);

		if (!hardwareInitializer_->hadError())
		{
			std::cout << hardwareInitializer_->initializerName() << " loaded." << std::endl;

			hardwareInitializer = hardwareInitializer_;
			inputProcessor = inputProcessor_;

			gfxApi = defaultGfxApi();
			gfxApi->setRenderDimensions(windowWidth, windowHeight);

			if (gfxApi->init(*hardwareInitializer))
			{
				gfxApi->initializeUBORanges();

				pbInitialized = true;

				assetLibrary = std::make_unique<AssetLibrary>("../", gfxApi);
				assetLibrary->init();
				LOGGER_DEBUG("GFX Api loaded.");
			}
			else
			{
				LOGGER_ERROR("Failed to initialize Engine");
			}
		}
		else
		{
			LOGGER_ERROR("Failed to initialize hardware");
		}
	}

	void CreateScene(std::string sceneName)
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

	void SetSceneCameraMode(std::string sceneName, SceneView::Mode mode)
	{
		if (loadedScenes.find(sceneName) != loadedScenes.end())
		{
			loadedScenes.at(sceneName)->setCameraMode(mode);
		}
		else
		{
			LOGGER_ERROR("Scene does not exist with this identifier name");
		}
	}

	void SetActiveScene(std::string sceneName)
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

	void LoadAssetPack(std::string archiveName)
	{
		assetLibrary->loadArchive(archiveName);
	}

	bool CreateSceneObject(std::string assetPath, SceneObject* sceneObject, LibraryAsset::Type type)
	{
		if (sceneObject == nullptr)
		{
			LOGGER_ERROR("SceneObject must be instantiated prior to invoking CreateSceneObject");
		}
		else
		{
			if (assetLibrary->loadAsset(assetPath, sceneObject, convertToAssetType(type)))
			{
				return true;
			}

			LOGGER_ERROR("Failed to load asset '" + assetPath + "'");
		}

		return false;
	}

	void Run()
	{
		Engine engine{ *gfxApi, *hardwareInitializer, *inputProcessor };

		engine.setScene(&activeScene());

		engine.run();

		engine.shutdown();
	}
}