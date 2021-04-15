#pragma once

#include "IHardwareInitializer.h"

namespace PB
{
	class IGfxApi
	{
	public:
		virtual bool load(const IHardwareInitializer& hardwareInitializer) const = 0;
		virtual void preLoopCommands() const = 0;
	};
}