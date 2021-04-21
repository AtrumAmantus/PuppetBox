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
#include "MessageBroker.h"
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
		std::unordered_map<std::string, AssetLibrary> assetLibraries{};
		MessageBroker messageBroker{};
		std::string activeScene;
		SceneGraph invalidScene{ "InvalidScene", messageBroker };
		bool pbInitialized = false;

		std::shared_ptr<IGfxApi> defaultGfxApi()
		{
			return std::make_shared<GladGfxApi>();
		}

		SceneGraph& ActiveScene()
		{
			if (!activeScene.empty() && loadedScenes.find(activeScene) != loadedScenes.end())
			{
				return *loadedScenes.at(activeScene);
			}

			return invalidScene;
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

			if (gfxApi->load(*hardwareInitializer))
			{
				pbInitialized = true;
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
			std::shared_ptr<SceneGraph> scene = std::make_shared<SceneGraph>(sceneName, messageBroker);
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
			activeScene = sceneName;
		}
		else
		{
			LOGGER_ERROR("Scene does not exist with this identifier name");
		}
	}

	void SetSceneHandler(AbstractSceneHandler* sceneHandler)
	{
		ActiveScene().setSceneHandler(sceneHandler);
	}

	void LoadAssetLibrary(std::string archiveName)
	{
		if (assetLibraries.find(archiveName) == assetLibraries.end())
		{
			AssetLibrary assetLibrary{ archiveName };
			assetLibrary.init();

			assetLibraries.insert(
				std::pair<std::string, AssetLibrary>{archiveName, assetLibrary}
			);
			
			LOGGER_DEBUG("'" + archiveName + "' loaded " + std::to_string(assetLibrary.assetCount()) + " assets");
		}
		else
		{
			LOGGER_WARN("Library '" + archiveName + "' was already loaded");
		}
	}

	void Run()
	{
		Engine engine{ *gfxApi, *hardwareInitializer, *inputProcessor };

		engine.setScene(&ActiveScene());

		if (engine.init())
		{
			engine.run();
		}
		else
		{
			LOGGER_ERROR("Engine stalled.");
		}
	}
}