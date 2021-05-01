#pragma once

#include <string>

#include "TypeDef.h"

namespace PB
{
	class IHardwareInitializer
	{
	public:
		virtual void init(std::string windowTitle, uint32_t windowWidth, uint32_t windowHeight) = 0;
		virtual void destroy() = 0;
		virtual bool hadError() const = 0;
		virtual void postLoopCommands() const = 0;
		virtual void initializeGameTime() = 0;
		virtual float updateElapsedTime() = 0;
		virtual ProcAddress getProcAddress() const = 0;
		virtual std::string initializerName() const = 0;
	};
}