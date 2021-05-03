#pragma once

#include <stdint.h>

#include "IHardwareInitializer.h"
#include "ImageData.h"
#include "ImageReference.h"
#include "Mesh.h"
#include "Vertex.h"

namespace PB
{
	/**
	* \brief Interface to define execution of GFX API specific logic.
	*/
	class IGfxApi
	{
	public:
		/**
		* \brief Used to initialize GFX API specific configurations.
		* 
		* \param hardwareInitializer	Reference to the current hardware library to be used for GFX API configurations.
		* 
		* \return True if the GFX API initialization was successful, False otherwise.
		*/
		virtual bool init(const IHardwareInitializer& hardwareInitializer) const = 0;
		
		/**
		* \brief Used to define GFX API specific commands that must execute before each rendering loop.
		*/
		virtual void preLoopCommands() const = 0;

		/**
		* \brief Used to execute GFX API specific command to set renderer dimensions.
		* 
		* \param width	The desired width of the rendering area.
		* \param height	The desired height of the rendering area.
		*/
		virtual void setRenderDimensions(uint32_t width, uint32_t height) = 0;

		/**
		* \brief Used to execute the GFX API specific commands to load an image into GFX memory.
		*
		* \param imageData	The image data to be loaded into memory.
		*/
		virtual ImageReference loadImage(ImageData imageData) const = 0;

		/**
		* \brief Used to execute the GFX API specific commands to load vertex data into GFX memory.
		*
		* \param vertexData		The vertex data to load into memory.
		* \param vertexCount	The number of entries in the vertexData array.
		*/
		virtual Mesh loadMesh(Vertex* vertexData, uint32_t vertexCount) const = 0;
	};
}