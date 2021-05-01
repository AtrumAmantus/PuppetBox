#include "pch.h"

#include <memory>
#include <string>

#include <PuppetBox.h>
#include <STBI/stb_image.h>

#include "AssetLibrary.h"
#include "AbstractInputProcessor.h"
#include "Engine.h"
#include "GladGfxApi.h"
#include "IGfxApi.h"
#include "IHardwareInitializer.h"
#include "KeyCode.h"
#include "SceneGraph.h"

#if defined(_USE_GLFW)
#	include "GlfwInitializer.h"
#	include "GlfwInputProcessor.h"
#elif defined(_USE_SDL2)
#	include "Sdl2Initializer.h"
#	include "Sdl2InputProcessor.h"
#else
#	error Must define ONE of GLFW or SDL2
#endif

namespace PB
{
	namespace
	{
		std::shared_ptr<IHardwareInitializer> hardwareInitializer{ nullptr };
		std::shared_ptr<AbstractInputProcessor> inputProcessor{ nullptr };
		std::shared_ptr<IGfxApi> gfxApi{ nullptr };
		std::unordered_map<std::string, std::shared_ptr<SceneGraph>> loadedScenes{};
		std::unique_ptr<AssetLibrary> assetLibrary{ nullptr };
		std::string activeSceneId;
		SceneGraph invalidScene{ "InvalidScene" };
		bool pbInitialized = false;

		std::shared_ptr<IGfxApi> defaultGfxApi()
		{
			return std::make_shared<GladGfxApi>();
		}

		SceneGraph& activeScene()
		{
			if (!activeSceneId.empty() && loadedScenes.find(activeSceneId) != loadedScenes.end())
			{
				return *loadedScenes.at(activeSceneId);
			}

			return invalidScene;
		}

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
	}

	void Init(std::string windowTitle, uint32_t windowWidth, uint32_t windowHeight)
	{
		Init_CharMap();

		// Create hardware Api instance
#ifdef _USE_SDL2
		std::shared_ptr<Sdl2InputProcessor> inputProcessor_ = std::make_shared<Sdl2InputProcessor>();
		std::shared_ptr<Sdl2Initializer> hardwareInitializer_ = std::make_shared<Sdl2Initializer>();
#endif //_USE_SDL2

#ifdef _USE_GLFW
		std::shared_ptr<GlfwInitializer> hardwareInitializer_ = std::make_shared<GlfwInitializer>();
#endif //_UES_GLFW

		hardwareInitializer_->init(windowTitle, windowWidth, windowHeight);

		if (!hardwareInitializer_->hadError())
		{
			std::cout << hardwareInitializer_->initializerName() << " loaded." << std::endl;

			// GLFW needs window reference for input polling
#ifdef _USE_GLFW
			std::shared_ptr<GlfwInputProcessor> inputProcessor_ = std::make_shared<GlfwInputProcessor>(hardwareInitializer_->getWindow());
			inputProcessor_->init(hardwareInitializer_->getWindowProperties());
#endif //_USE_GLFW

			hardwareInitializer = hardwareInitializer_;
			inputProcessor = inputProcessor_;

			gfxApi = defaultGfxApi();
			gfxApi->setRenderDimensions(windowWidth, windowHeight);

			if (gfxApi->init(*hardwareInitializer))
			{
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
			std::shared_ptr<SceneGraph> scene = std::make_shared<SceneGraph>(sceneName);
			scene->init();

			loadedScenes.insert(
				std::pair<std::string, std::shared_ptr<SceneGraph>>{sceneName, scene}
			);
		}
		else
		{
			LOGGER_ERROR("Scene already exists with this identifier name");
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

	void CreateSceneObject(std::string assetPath, SceneObject* sceneObject, LibraryAsset::Type type)
	{
		if (sceneObject == nullptr)
		{
			LOGGER_ERROR("SceneObject must be instantiated prior to invoking CreateSceneObject");
		}
		else
		{
			if (!assetLibrary->loadAsset(assetPath, sceneObject, convertToAssetType(type)))
			{
				LOGGER_ERROR("Failed to load asset '" + assetPath + "'");
			}
		}
	}

	void Run()
	{
		Engine engine{ *gfxApi, *hardwareInitializer, *inputProcessor };

		engine.setScene(&activeScene());

		if (engine.init())
		{
			engine.run();
		}
		else
		{
			LOGGER_ERROR("Engine stalled.");
		}

		engine.shutdown();
	}
}