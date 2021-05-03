#pragma once

#include <iostream>

#include <glad/glad.h>

#include "IGfxApi.h"
#include "IHardwareInitializer.h"
#include "ImageOptions.h"
#include "ImageReference.h"
#include "Logger.h"
#include "Mesh.h"
#include "TypeDef.h"
#include "Vertex.h"

namespace PB
{
	/**
	* \brief The OpenGL API specific implementations for the IGfxApi interface.
	*/
	class OpenGLGfxApi : public IGfxApi
	{
	public:
		/**
		* \brief Used to initialize OpenGL API specific configurations.
		*
		* \param hardwareInitializer	Reference to the current hardware library to be used for OpenGL API configurations.
		*
		* \return True if the OpenGL API initialization was successful, False otherwise.
		*/
		bool init(const IHardwareInitializer& hardwareInitializer) const;

		/**
		* \brief Used to define OpenGL API specific commands that must execute before each rendering loop.
		*/
		void preLoopCommands() const;

		/**
		* \brief Used to execute OpenGL API specific command to set renderer dimensions.
		*
		* \param width	The desired width of the rendering area.
		* \param height	The desired height of the rendering area.
		*/
		void setRenderDimensions(uint32_t width, uint32_t height);

		/**
		* \brief Used to execute the OpenGL API specific commands to load an image into GFX memory.
		*
		* \param imageData	The image data to be loaded into memory.
		* \param options	Image loading options to use when loading into memory.
		*/
		ImageReference loadImage(ImageData imageData, ImageOptions options = {}) const;

		/**
		* \brief Used to execute the OpenGL API specific commands to load vertex data into GFX memory.
		*
		* \param vertexData		The vertex data to load into memory.
		* \param vertexCount	The number of entries in the vertexData array.
		*/
		Mesh loadMesh(Vertex* vertexData, uint32_t vertexCount) const;
	private:
		uint32_t width_ = 0;
		uint32_t height_ = 0;
	};
}
