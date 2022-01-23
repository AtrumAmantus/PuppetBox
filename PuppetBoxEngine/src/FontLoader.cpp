#include "FontLoader.h"

#include <utility>

namespace PB
{

    FontLoader::FontLoader(std::shared_ptr<IGfxApi> gfxApi) : gfxApi_(std::move(gfxApi))
    {

    }

    Font FontLoader::loadFontFromBytes(SizedArray<char> bytes, std::uint32_t fontSize, bool* error)
    {
        Font font{};

        //TODO: This should probably not be initialized every time
        FT_Library library;

        if (!FT_Init_FreeType(&library))
        {
            FT_Face face;

            FT_Error ftResultCode = FT_New_Memory_Face(library, (FT_Byte*) bytes.array, bytes.length, 0, &face);

            if (ftResultCode != 0)
            {
                *error = true;
                LOGGER_ERROR("Failed to load specified font face: FT_Error = " + std::to_string(ftResultCode));
            }
            else
            {
                FT_Set_Pixel_Sizes(face, 0, fontSize);

                std::unordered_map<std::int8_t, TypeCharacter> characterMap{};
                *error = !gfxApi_->buildCharacterMap(face, characterMap);
                font = Font{characterMap, fontSize};
            }

            FT_Done_Face(face);
        }
        else
        {
            *error = true;
            LOGGER_ERROR("FreeType library was not initialized, or failed to initialize");
        }

        FT_Done_FreeType(library);

        return font;
    }
}