#include "pch.h"

#include <memory>
#include <string>

#include <PuppetBox.h>

#include "AbstractInputProcessor.h"
#include "Engine.h"
#include "IGfxApi.h"
#include "IHardwareInitializer.h"
#include "KeyCode.h"
#include "GladGfxApi.h"

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
		std::shared_ptr<IHardwareInitializer> hardwareInitializer = nullptr;
		std::shared_ptr<AbstractInputProcessor> inputProcessor = nullptr;
		std::shared_ptr<IGfxApi> gfxApi = nullptr;

		std::shared_ptr<IGfxApi> defaultGfxApi()
		{
			return std::make_shared<GladGfxApi>();
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
				LOGGER_DEBUG("GFX Api loaded.");

				Engine engine{ *gfxApi, *hardwareInitializer, *inputProcessor };

				if (engine.init())
				{
					engine.run();
				}
				else
				{
					LOGGER_ERROR("Failed to initialize Engine");
				}
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
}