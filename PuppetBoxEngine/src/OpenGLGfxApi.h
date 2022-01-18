#pragma once

#include <cstdint>

#include <iostream>

#include <glad/glad.h>

#include "puppetbox/DataStructures.h"

#include "IGfxApi.h"
#include "IHardwareInitializer.h"
#include "ImageOptions.h"
#include "ImageReference.h"
#include "Logger.h"
#include "Mesh.h"
#include "TypeDef.h"

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
        * \param procAddress	Reference to the ProcAddress function for loading gfx function pointers.
        *
        * \return True if the OpenGL API initialization was successful, False otherwise.
        */
        bool init(const PB::ProcAddress procAddress) override;

        /**
        * \brief Used to define OpenGL API specific commands that must execute before each rendering loop.
        */
        void preLoopCommands() const override;

        /**
        * \brief Used to execute OpenGL API specific command to set renderer dimensions.
        *
        * \param width	The desired width of the rendering area.
        * \param height	The desired height of the rendering area.
        */
        void setRenderDimensions(std::uint32_t width, std::uint32_t height) override;

        /**
        * \brief Get the current render window area's width.
        */
        std::uint32_t getRenderWidth() override;

        /**
        * \brief Get the current render window area's height.
        */
        std::uint32_t getRenderHeight() override;

        /**
        * \brief Used to execute the OpenGL API specific commands to load an image into GFX memory.
        *
        * \param imageData	The image data to be loaded into memory.
        * \param options	Image loading options to use when loading into memory.
        */
        ImageReference loadImage(ImageData imageData, ImageOptions options) const override;

        /**
        * \brief Used to execute the OpenGL API specific commands to load vertex data into GFX memory.
        *
        * \param vertexData		The vertex data to load into memory.
        * \param vertexCount	The number of entries in the vertexData array.
        */
        Mesh loadMesh(Vertex* vertexData, std::uint32_t vertexCount) const override;

        /**
        * \brief Initializes the UBO buffer, defining the data ranges.  This is needed before use.
        */
        void initializeUBORanges() override;

        /**
        * \brief Sets view/projection matrices in the UBO.  Must initializeUBORanges() before use.
        *
        * \param view		        The View Matrix to set.
        * \param projection	        The Projection Matrix to set.
        * \param orthoProjection    The orthographic projection matrix to set.
        */
        void setTransformUBOData(mat4 view, mat4 projection, mat4 orthoProjection) const override;

        /**
        * \brief Enables the OpenGL API debugger.
        *
        * \return True if the debugger was enabled, False otherwise.
        */
        bool initGfxDebug() const override;

    private:
        std::uint32_t width_ = 0;
        std::uint32_t height_ = 0;
        std::uint32_t UBO_ = 0;
        std::uint32_t minimumUBOOffset_ = 0;
    };
}
