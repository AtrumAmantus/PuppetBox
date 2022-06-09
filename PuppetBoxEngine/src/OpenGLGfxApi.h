#pragma once

#include <cstdint>

#include "puppetbox/DataStructures.h"
#include "puppetbox/RenderWindow.h"

#include "IGfxApi.h"
#include "ImageOptions.h"
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
        bool init(PB::ProcAddress procAddress) override;

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
         * \brief Sets the render distance of the render window, defining the z-depth range of
         * elements that will be rendered.
         *
         * \param distance The distance on the z-axis of elements to render.
         */
        void setRenderDistance(std::uint32_t distance) override;

        /**
         * \brief Returns an object containing references to the window dimensions.
         *
         *
         * \return Object holding references to the window dimensions.
         */
        const RenderWindow getRenderWindow() override;

        std::uint32_t loadImage(ImageData imageData) const override;

        /**
         * \brief Generates textures for each glyph of the given font face, adding references to them
         * within the loaded characters {\link unordered_map}.
         *
         * \param face              The font face to generate textures for.
         * \param loadedCharacters  The {\link unordered_map} to store texture references in for the generated
         * glyph images.
         * \param fontSize          The requested font size to load the font at.
         *
         * \return True if the glyph textures were successfully generated and loaded, False otherwise.
         */
        bool buildCharacterMap(
                FT_Face face,
                std::unordered_map<std::int8_t, TypeCharacter>& loadedCharacters) const override;

        /**
        * \brief Used to execute the OpenGL API specific commands to load vertex data into GFX memory.
        *
        * \param vertexData		The vertex data to load into memory.
        * \param vertexCount	The number of entries in the vertexData array.
        *
        * \return Reference to the render details to use for rendering this mesh
        */
        std::uint32_t loadMesh(
                Vertex* vertexData,
                std::uint32_t vertexCount,
                std::vector<std::vector<BoneWeight>> meshBoneWeights) const override;

        std::uint32_t loadShader(const Shader& shader) const override;

        /**
        * \brief Initializes the UBO buffer, defining the data ranges.  This is needed before use.
        */
        void initializeUBORanges() override;

        /**
        * \brief Sets view/projection matrices in the UBO.  Must initializeUBORanges() before use.
        *
        * \param view		     The View Matrix to set.
        * \param projection	     The Projection Matrix to set.
        * \param uiProjection    The UI orthographic projection matrix to set.
        */
        void setTransformUBOData(mat4 view, mat4 projection, mat4 uiProjection) const override;

        /**
        * \brief Enables the OpenGL API debugger.
        *
        * \return True if the debugger was enabled, False otherwise.
        */
        bool initGfxDebug() const override;

        std::unique_ptr<IRenderComponent> createRenderComponent() const override;

    private:
        std::uint32_t width_ = 0;
        std::uint32_t height_ = 0;
        std::uint32_t distance_ = 0;
        std::uint32_t UBO_ = 0;
        std::uint32_t minimumUBOOffset_ = 0;
    };
}
