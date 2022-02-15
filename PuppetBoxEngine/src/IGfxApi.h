#pragma once

#include <cstdint>

//TODO: This is coupled to the FreeType library and it shouldn't be.
#include <ft2build.h>
#include FT_FREETYPE_H

#include "puppetbox/DataStructures.h"

#include "Font.h"
#include "ImageData.h"
#include "ImageOptions.h"
#include "ImageReference.h"
#include "Mesh.h"
#include "TypeDef.h"

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
        * \param procAddress	Reference to the ProcAddress function for loading gfx function pointers.
        *
        * \return True if the GFX API initialization was successful, False otherwise.
        */
        virtual bool init(PB::ProcAddress procAddress) = 0;

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

        virtual void setRenderDistance(std::uint32_t distance) = 0;

        /**
        * \brief Get the current render window area's width.
        */
        virtual std::uint32_t getRenderWidth() const = 0;

        /**
        * \brief Get the current render window area's height.
        */
        virtual std::uint32_t getRenderHeight() const = 0;

        /**
         * \brief Get the current render distance for the scene.
         */
        virtual std::uint32_t getRenderDistance() const = 0;

        /**
        * \brief Used to execute the GFX API specific commands to load an image into GFX memory.
        *
        * \param imageData	The image data to be loaded into memory.
        * \param options	Image loading options to use when loading into memory.
        */
        virtual ImageReference loadImage(ImageData imageData, ImageOptions options) const = 0;

        /**
         * \brief Generates textures for each glyph of the given font face, adding references to them
         * within the loaded characters {\link unordered_map}.
         *
         * @param face              The font face to generate textures for.
         * @param loadedCharacters  The {\link unordered_map} to store texture references in for the generated
         * glyph images.
         * @return True if the glyph textures were successfully generated and loaded, False otherwise.
         */
        virtual bool
        buildCharacterMap(FT_Face face, std::unordered_map<int8_t, TypeCharacter>& loadedCharacters) const = 0;

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
        * \param view		     The View Matrix to set.
        * \param projection	     The Projection Matrix to set.
        * \param uiProjection    The UI orthographic projection matrix to set.
        */
        virtual void setTransformUBOData(mat4 view, mat4 projection, mat4 uiProjection) const = 0;

        /**
        * \brief Enables GFX API Debugging if it has one.
        *
        * \return True if the debugger was found and enabled, False otherwise.
        */
        virtual bool initGfxDebug() const = 0;
    };
}