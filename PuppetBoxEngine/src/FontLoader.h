#pragma once

#include <unordered_map>
#include <utility>

#include <glad/glad.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "puppetbox/DataStructures.h"
#include "Logger.h"
#include "Shader.h"

#define COLOUR_WHITE { 1.0f, 1.0f, 1.0f }

namespace PB
{
    struct TypeCharacter
    {
        std::uint32_t textureId;
        uivec2 size;
        ivec2 bearing;
        std::uint32_t advance;
    };

    class FontLoader
    {
    public:
        explicit FontLoader(Shader shader) : shader_(std::move(shader)) {};

        bool init()
        {
            bool success;

            buildQuad();

            if (FT_Init_FreeType(&library_))
            {
                success = false;
                LOGGER_ERROR("Failed to load freetype library");
            }
            else
            {
                if (FT_New_Face(library_, "../fonts/arial.ttf", 0, &face_))
                {
                    success = false;
                    LOGGER_ERROR("Failed to load specified font face");
                }
                else
                {
                    FT_Set_Pixel_Sizes(face_, 0, fontHeight_);

                    success = buildCharacterMap();
                }
            }

            FT_Done_Face(face_);
            FT_Done_FreeType(library_);

            return success;
        };

        void RenderText(std::string text, float x, float y, float scale, vec3 colour)
        {
            if (VAO_ != 0)
            {
                shader_.use();
                shader_.setVec3("textColour", colour);

                glEnable(GL_BLEND);
                glActiveTexture(GL_TEXTURE0);
                glBindVertexArray(VAO_);

                float originalX = x;

                std::string::const_iterator c;
                for (c = text.begin(); c != text.end(); ++c)
                {
                    if (*c == '\n')
                    {
                        x = originalX;
                        y -= fontHeight_ + 2;
                    }
                    else
                    {
                        TypeCharacter tchar = loadedCharacters_[*c];

                        float xpos = x + tchar.bearing.x * scale;
                        float ypos = y - (tchar.size.y - tchar.bearing.y) * scale;

                        float w = tchar.size.x * scale;
                        float h = tchar.size.y * scale;
                        // update VBO for each character
                        float vertices[6][4] = {
                                {xpos,     ypos + h, 0.0f, 0.0f},
                                {xpos,     ypos,     0.0f, 1.0f},
                                {xpos + w, ypos,     1.0f, 1.0f},

                                {xpos,     ypos + h, 0.0f, 0.0f},
                                {xpos + w, ypos,     1.0f, 1.0f},
                                {xpos + w, ypos + h, 1.0f, 0.0f}
                        };
                        // render glyph texture over quad
                        glBindTexture(GL_TEXTURE_2D, tchar.textureId);
                        // update content of VBO memory
                        glBindBuffer(GL_ARRAY_BUFFER, VBO_);
                        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
                        glBindBuffer(GL_ARRAY_BUFFER, 0);
                        // render quad
                        glDrawArrays(GL_TRIANGLES, 0, 6);
                        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
                        x += (tchar.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
                    }
                }

                shader_.unuse();
            }
            else
            {
                LOGGER_ERROR("FontLoader was not initialized.");
            }

            glDisable(GL_BLEND);
        }

    private:
        Shader shader_;
        std::uint32_t fontHeight_ = 72;
        std::unordered_map<int8_t, TypeCharacter> loadedCharacters_{};
        FT_Library library_{};
        FT_Face face_{};
        std::uint32_t VAO_ = 0;
        std::uint32_t VBO_ = 0;
    private:
        void buildQuad()
        {
            glGenVertexArrays(1, &VAO_);
            glGenBuffers(1, &VBO_);

            glBindVertexArray(VAO_);
            glBindBuffer(GL_ARRAY_BUFFER, VBO_);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*) 0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        };

        bool buildCharacterMap()
        {
            bool success = true;

            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);  // Disable byte-alignment restriction

            for (std::uint8_t c = 0; success && c < 128U; ++c)
            {
                if (FT_Load_Char(face_, c, FT_LOAD_RENDER))
                {
                    success = false;
                    LOGGER_ERROR("Failed to load glyph for '" + std::to_string(static_cast<char>(c)) + "'");
                }
                else
                {
                    std::uint32_t texture;
                    glGenTextures(1, &texture);
                    glBindTexture(GL_TEXTURE_2D, texture);
                    glTexImage2D(
                            GL_TEXTURE_2D,
                            0,
                            GL_RED,
                            face_->glyph->bitmap.width,
                            face_->glyph->bitmap.rows,
                            0,
                            GL_RED,
                            GL_UNSIGNED_BYTE,
                            face_->glyph->bitmap.buffer
                    );

                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                    loadedCharacters_.insert(
                            std::pair<int8_t, TypeCharacter>{
                                    c,
                                    TypeCharacter{
                                            texture,
                                            {face_->glyph->bitmap.width, face_->glyph->bitmap.rows},
                                            {face_->glyph->bitmap_left, face_->glyph->bitmap_top},
                                            static_cast<std::uint32_t>(face_->glyph->advance.x)
                                    }
                            }
                    );
                }
            }

            return success;
        };
    };
};