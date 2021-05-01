#pragma once

#include <iostream>

#include <glad/glad.h>

#include "IGfxApi.h"
#include "IHardwareInitializer.h"
#include "ImageReference.h"
#include "Logger.h"
#include "Mesh.h"
#include "TypeDef.h"
#include "Vertex.h"

namespace PB
{
	class GladGfxApi : public IGfxApi
	{
	public:
		bool init(const IHardwareInitializer& hardwareInitializer) const;
		void preLoopCommands() const;
		void setRenderDimensions(uint32_t width, uint32_t height);
		ImageReference loadImage(ImageData imageData) const;
		Mesh loadMesh(Vertex* vertexData, uint32_t vertexCount) const;
	private:
		uint32_t width_ = 0;
		uint32_t height_ = 0;
	};
}
