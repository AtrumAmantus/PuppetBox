#pragma once

#include <stdint.h>

#include "IHardwareInitializer.h"
#include "ImageData.h"
#include "ImageReference.h"
#include "Mesh.h"
#include "Vertex.h"

namespace PB
{
	class IGfxApi
	{
	public:
		virtual bool init(const IHardwareInitializer& hardwareInitializer) const = 0;
		virtual void preLoopCommands() const = 0;
		virtual void setRenderDimensions(uint32_t width, uint32_t height) = 0;
		virtual ImageReference loadImage(ImageData imageData) const = 0;
		virtual Mesh loadMesh(Vertex* vertexData, uint32_t vertexCount) const = 0;
	};
}