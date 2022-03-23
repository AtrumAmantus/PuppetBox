#pragma once

#include <unordered_map>
#include <utility>

#include <glad/glad.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "puppetbox/DataStructures.h"
#include "Font.h"
#include "IGfxApi.h"
#include "Logger.h"
#include "Shader.h"

#define COLOUR_WHITE { 1.0f, 1.0f, 1.0f }

namespace PB
{
    class FontLoader
    {
    public:
        explicit FontLoader(std::shared_ptr<IGfxApi> gfxApi);

        /**
         * \brief Loads font data from the given {\link SizedArray}.
         *
         * \param bytes     The {\link SizedArray} containing font file byte data.
         * \param fontSize  The desired font size to use when creating glyph render references.
         * \param error     Error flag indicating if there was an issue loading the font.
         * \return A {\link Font} object containing references to the renderable glyphs of the loaded font.
         */
        Font loadFontFromBytes(SizedArray<std::uint8_t> bytes, std::uint32_t fontSize, bool* error);

    private:
        std::shared_ptr<IGfxApi> gfxApi_;
    };
};