#pragma once

#include <cstdint>

#include "puppetbox/DataStructures.h"

#include "IHardwareInitializer.h"
#include "ImageData.h"
#include "ImageOptions.h"
#include "ImageReference.h"
#include "Mesh.h"

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
		virtual bool init(const IHardwareInitializer& hardwareInitializer) = 0;
		
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
		virtual void setRenderDimensions(std::uint32_t width, std::uint32_t height) = 0;

		/**
		* \brief Get the current render window area's width.
		*/
		virtual std::uint32_t getRenderWidth() = 0;

		/**
		* \brief Get the current render window area's height.
		*/
		virtual std::uint32_t getRenderHeight() = 0;

		/**
		* \brief Used to execute the GFX API specific commands to load an image into GFX memory.
		*
		* \param imageData	The image data to be loaded into memory.
		* \param options	Image loading options to use when loading into memory.
		*/
		virtual ImageReference loadImage(ImageData imageData, ImageOptions options) const = 0;

		/**
		* \brief Used to execute the GFX API specific commands to load vertex data into GFX memory.
		*
		* \param vertexData		The vertex data to load into memory.
		* \param vertexCount	The number of entries in the vertexData array.
		*/
		virtual Mesh loadMesh(Vertex* vertexData, std::uint32_t vertexCount) const = 0;

		/**
		* \brief Initializes the UBO buffer, defining the data ranges.  This is needed before use.
		*/
		virtual void initializeUBORanges() = 0;

		/**
		* \brief Sets view/projection matrices in the UBO.  Must initializeUBORanges() before use.
		*
		* \param view		The View Matrix to set.
		* \param projection	The Projection Matrix to set.
		*/
		virtual void setTransformUBOData(mat4 view, mat4 projection) const = 0;
	};
}