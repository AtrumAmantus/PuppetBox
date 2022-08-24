#include <cassert>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include <ft2build.h>
#include FT_FREETYPE_H

#define BIG_ENDIAN 0

std::uint8_t getHostEndian()
{
    std::uint32_t n = 1;
    auto p = (uint8_t*) &n;
    return *p;
}

bool isHostBigEndian = getHostEndian() == BIG_ENDIAN;

const std::uint32_t endianInt32Byte0 = isHostBigEndian ? 0 : 3;
const std::uint32_t endianInt32Byte1 = isHostBigEndian ? 1 : 2;
const std::uint32_t endianInt32Byte2 = isHostBigEndian ? 2 : 1;
const std::uint32_t endianInt32Byte3 = isHostBigEndian ? 3 : 0;

const std::uint8_t endianInt32Shift0 = isHostBigEndian ? 0 : 24;
const std::uint8_t endianInt32Shift1 = isHostBigEndian ? 8 : 16;
const std::uint8_t endianInt32Shift2 = isHostBigEndian ? 16 : 8;
const std::uint8_t endianInt32Shift3 = isHostBigEndian ? 24 : 0;

struct ivec2
{
    union
    {
        std::int32_t x, r, s = 0;
    };
    union
    {
        std::int32_t y, g, t = 0;
    };

    std::int32_t& operator[](std::uint32_t i)
    {
        assert(i < 2);
        return (&x)[i];
    }

    const std::int32_t& operator[](std::uint32_t i) const
    {
        assert(i < 2);
        return (&x)[i];
    }

    ivec2 operator*(const std::int32_t& scalar) const
    {
        return {
                this->x * scalar,
                this->y * scalar
        };
    };

    ivec2 operator-(const ivec2& rhv) const
    {
        return {
                this->x - rhv.x,
                this->y - rhv.y
        };
    };
};

struct uivec2
{
    union
    {
        std::uint32_t x, r, s = 0;
    };
    union
    {
        std::uint32_t y, g, t = 0;
    };

    std::uint32_t& operator[](std::uint32_t i)
    {
        assert(i < 2);
        return (&x)[i];
    }

    const std::uint32_t& operator[](std::uint32_t i) const
    {
        assert(i < 2);
        return (&x)[i];
    }
};

/**
 * \brief Similar to Java's "Optional" class, this is used to indicate if the
 * response had a result.
 *
 * \tparam T The type of data being stored in the result.
 */
template<typename T>
struct Result
{
    bool hasResult = false;
    T result;
};

template<typename T>
struct SizedArray
{
    T* array = nullptr;
    std::uint32_t length = 0;
};

/**
 * Holds information related to a particular glyph, including a reference for rendering.
 */
struct TypeCharacter
{
    std::uint8_t character = 0;
    uivec2 size{};
    ivec2 bearing{};
    std::uint32_t advance = 0;
    std::uint8_t data[];
};

bool buildFontSet(const FT_Face& face, const std::vector<char>& characters, std::unordered_map<std::int8_t, std::unique_ptr<TypeCharacter>>& characterMap)
{
    bool success = true;

    for (std::uint8_t c : characters) {

        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            success = false;
            std::cout << "Failed to load glyph for '" << std::to_string(static_cast<char>(c)) << "'";
        }
        else
        {
            /*
             * OpenGL uses the bottom left corner as the point of origin,
             * so we need to invert the "rows" of byte data, since image
             * data on the host machine is stored with the origin in the
             * top left corner.
             */
            std::uint32_t bitmapWidth = face->glyph->bitmap.width;
            std::uint32_t bitmapHeight = face->glyph->bitmap.rows;

            for (std::uint32_t row = 0; row < bitmapHeight / 2; ++row)
            {
                for (std::uint32_t column = 0; column < bitmapWidth; ++column)
                {
                    std::uint32_t currentIndex = (bitmapWidth * row) + column;
                    std::uint32_t invertedIndex = (bitmapWidth * (bitmapHeight - row - 1)) + column;

                    std::uint8_t tmp = face->glyph->bitmap.buffer[currentIndex];
                    face->glyph->bitmap.buffer[currentIndex] = face->glyph->bitmap.buffer[invertedIndex];
                    face->glyph->bitmap.buffer[invertedIndex] = tmp;
                }
            }

            // Now organize all the data to be written
            auto* typeChar = (TypeCharacter*) malloc(sizeof(TypeCharacter) + (bitmapWidth * bitmapHeight));
            typeChar->character = c;
            typeChar->size = {bitmapWidth, bitmapHeight};
            typeChar->bearing = {face->glyph->bitmap_left, face->glyph->bitmap_top};
            typeChar->advance = static_cast<std::uint32_t>(face->glyph->advance.x);
            std::uint32_t bufferSize = bitmapWidth * bitmapHeight;

            for (std::uint32_t j = 0; j < bufferSize; ++j) {
                typeChar->data[j] = face->glyph->bitmap.buffer[j];
            }

            characterMap.insert(std::pair<std::uint8_t, TypeCharacter*>{c, typeChar});
        }
    }

    return success;
}

SizedArray<std::uint8_t> loadFileBytes(const std::string& fileName)
{
    std::ifstream input(fileName, std::ios::binary);

    std::vector<char> bytesVector(
            (std::istreambuf_iterator<char>(input)),
            (std::istreambuf_iterator<char>()));

    input.close();

    auto bytes = new std::uint8_t[bytesVector.size()];

    for (std::uint32_t i = 0; i < bytesVector.size(); ++i)
    {
        bytes[i] = bytesVector[i];
    }

    return {
        bytes,
        bytesVector.size()
    };
}

std::unordered_map<std::int8_t, std::unique_ptr<TypeCharacter>> loadFont(SizedArray<std::uint8_t> bytes, std::uint32_t fontSize, bool* error)
{
    std::unordered_map<std::int8_t, std::unique_ptr<TypeCharacter>> characterMap{};

    //TODO: This should probably not be initialized every time
    FT_Library library;

    if (!FT_Init_FreeType(&library))
    {
        FT_Face face;

        FT_Error ftResultCode = FT_New_Memory_Face(library, (FT_Byte*) bytes.array, bytes.length, 0, &face);

        if (ftResultCode != 0)
        {
            *error = true;
            std::cout << "Failed to load specified font face: FT_Error = " << std::to_string(ftResultCode);
        }
        else
        {
            FT_Set_Pixel_Sizes(face, 0, fontSize);

            std::vector<char> charactersToRender{};

            // The first 32 ascii values are not needed
            for (std::uint32_t i = 32; i < 128; ++i)
            {
                charactersToRender.emplace_back(i);
            }

            *error = buildFontSet(face, charactersToRender, characterMap);
        }

        FT_Done_Face(face);
    }
    else
    {
        *error = true;
        std::cout << "FreeType library was not initialized, or failed to initialize";
    }

    FT_Done_FreeType(library);

    return characterMap;
}

struct Config
{
    bool isDirectory = false;
    std::string target;
};

Config loadRunConfig(std::uint32_t count, char** params)
{
    Config config{};

    if (count > 1) {
        const std::string& param = params[1];

        config.target = param;
        config.isDirectory = std::filesystem::is_directory(std::filesystem::path{param});
    }

    return config;
}

std::string convertToOutput(const std::string& fileName)
{
    std::filesystem::path path{fileName};
    path.replace_extension("pbf");
    return path.string();
}

void getBytesFromInt32(std::uint8_t bytes[4], std::int32_t value)
{
    auto valueBytes = reinterpret_cast<std::uint8_t*>(&value);

    bytes[0] = valueBytes[endianInt32Byte0];
    bytes[1] = valueBytes[endianInt32Byte1];
    bytes[2] = valueBytes[endianInt32Byte2];
    bytes[3] = valueBytes[endianInt32Byte3];
}

void getBytesFromUInt32(std::uint8_t bytes[4], std::uint32_t value)
{
    auto valueBytes = reinterpret_cast<std::uint8_t*>(&value);

    bytes[0] = valueBytes[endianInt32Byte0];
    bytes[1] = valueBytes[endianInt32Byte1];
    bytes[2] = valueBytes[endianInt32Byte2];
    bytes[3] = valueBytes[endianInt32Byte3];
}

void writeToOutputFile(const std::string& outputFile, const std::unordered_map<std::int8_t, std::unique_ptr<TypeCharacter>>& map)
{
    std::ofstream out;
    out.open(outputFile, std::ios::binary | std::ios::out);

    for (const auto& pair : map)
    {
        std::uint8_t bytes[4];
        const auto& typeChar = pair.second;
        out.write((char*) &typeChar->character, 1);
        getBytesFromInt32(bytes, typeChar->bearing.x);
        out.write((char*) &bytes, 4);
        getBytesFromInt32(bytes, typeChar->bearing.y);
        out.write((char*) &bytes, 4);
        getBytesFromUInt32(bytes, typeChar->advance);
        out.write((char*) &bytes, 4);
        getBytesFromUInt32(bytes, typeChar->size.x);
        out.write((char*) &bytes, 4);
        getBytesFromUInt32(bytes, typeChar->size.y);
        out.write((char*) &bytes, 4);
        std::uint32_t dataSize = typeChar->size.x * typeChar->size.y;
        out.write((char*) &typeChar->data, dataSize);
    }

    out.close();
}

int main(int argc, char* argv[])
{
    Config config = loadRunConfig(argc, argv);

    std::vector<std::string> files{};

    if (config.isDirectory)
    {
        for (const auto& entry : std::filesystem::directory_iterator(config.target))
        {
            std::filesystem::path ext = entry.path().extension();
            if (ext.string() == "ttf" || ext.string() == ".ttf")
            {
                files.emplace_back(entry.path().string());
            }
        }
    }
    else
    {
        files.emplace_back(config.target);
    }

    bool error = false;

    for (const auto& fileName : files)
    {
        auto fileBytes = loadFileBytes(fileName);
        auto map = loadFont(fileBytes, 12, &error);
        writeToOutputFile(convertToOutput(fileName), map);
    }

    return 0;
}
