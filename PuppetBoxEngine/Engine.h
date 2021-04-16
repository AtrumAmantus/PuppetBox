#pragma once

#include <sdl2/SDL.h>
//#include <gl/GL.h>

#include "AbstractInputProcessor.h"
#include "IGfxApi.h"
#include "IHardwareInitializer.h"

namespace PB
{
	class Engine
	{
	public:
		Engine(IGfxApi* gfxApi, IHardwareInitializer* hardwareInitializer, AbstractInputProcessor* inputProccessor) :
			gfxApi_(gfxApi), hardwareInitializer_(hardwareInitializer), inputProcessor_(inputProccessor) {};
		bool init();
		void run();
		void shutdown();
	private:
		IGfxApi* gfxApi_;
		IHardwareInitializer* hardwareInitializer_;
		AbstractInputProcessor* inputProcessor_;
	private:
		void loop();
		void processInput();
	};
}
