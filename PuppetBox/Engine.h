#pragma once

#include <sdl2/SDL.h>
//#include <gl/GL.h>

#include "AbstractInputProcessor.h"
#include "IHardwareInitializer.h"

class Engine
{
public:
	Engine(IHardwareInitializer* hardwareInitializer, AbstractInputProcessor* inputProccessor) :
		hardwareInitializer_(hardwareInitializer), inputProcessor_(inputProccessor) {};
	bool init();
	void run();
	void shutdown();
private:
	IHardwareInitializer* hardwareInitializer_;
	AbstractInputProcessor* inputProcessor_;
private:
	void loop();
	void processInput();
};

