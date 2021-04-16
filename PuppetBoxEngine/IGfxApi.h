#pragma once

#include "IHardwareInitializer.h"

namespace PB
{
	class IGfxApi
	{
	public:
		virtual bool load(const IHardwareInitializer& hardwareInitializer) const = 0;
		virtual void preLoopCommands() const = 0;
		virtual void setRenderDimensions(uint32_t width, uint32_t height) = 0;
	};
}