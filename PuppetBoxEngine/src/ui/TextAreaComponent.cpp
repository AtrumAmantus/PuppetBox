#pragma once

#include "../UIComponents.h"

namespace PB
{
    TextAreaComponent::TextAreaComponent(
            const std::shared_ptr<AssetLibrary>& assetLibrary,
            const std::shared_ptr<IGfxApi>& gfxApi
    ) : GfxUIComponent(assetLibrary, gfxApi)
    {

    }

    void TextAreaComponent::update(float deltaTime)
    {
        bool error = false;
        if (!font_.isInitialized())
        {
            Result<std::string> fontName = UIComponent::getStringAttribute(UI::FONT_TYPE);

            if (fontName.hasResult)
            {
                font_ = library()->loadFontAsset(fontName.result, 0, &error);
            }
        }
    }

    void TextAreaComponent::render() const
    {
        //TODO: Still need to calculate dimensions of text area, text wrap, etc.

        //TODO: This is a lot of calculations for the "render" phase, this should be moved to "Update".

        //TODO: Abstract the OpenGL calls/logic away
        bool error = false;
        //TODO: This shouldn't be pulled every render
        Shader shader = library()->loadShaderAsset("Assets1/Shaders/UI/Glyph", &error);

        if (!error)
        {
            std::string text = UIComponent::getStringAttribute(UI::TEXT_CONTENT).orElse("");

            if (!text.empty())
            {
                float x = UIComponent::getUIntAttribute(UI::POS_X).orElse(0);
                float y = UIComponent::getUIntAttribute(UI::POS_Y).orElse(0);
                float z = UIComponent::getUIntAttribute(UI::POS_Z).orElse(0);
                std::int32_t fontHeight = UIComponent::getUIntAttribute(UI::FONT_SIZE).orElse(0);
                std::string fontPath = UIComponent::getStringAttribute(UI::FONT_TYPE).orElse("");
                std::uint32_t origin = UIComponent::getUIntAttribute(UI::ORIGIN).orElse(UI::Origin::BOTTOM_LEFT);
                float scale = (float) fontHeight / font_.fontSize();

                shader.use();
                shader.setVec3("textColour", vec3{1.0f, 1.0f, 1.0f});

                Mesh mesh = library()->loadMeshAsset("Assets1/Mesh/Glyph", &error);
                glBindVertexArray(mesh.VAO);

                float originalX = x;

                std::string::const_iterator c;
                for (c = text.begin(); c != text.end(); ++c)
                {
                    if (*c == '\n')
                    {
                        x = originalX;
                        y -= fontHeight + 2;
                    }
                    else
                    {
                        TypeCharacter tchar = font_.getCharacter(*c).result;

                        float xpos = x + tchar.bearing.x * scale;
                        float ypos = y - (tchar.size.y - tchar.bearing.y) * scale;
                        float zpos = z;

                        float w = tchar.size.x * scale;
                        float h = tchar.size.y * scale;

                        bool originTop = origin == UI::TOP_LEFT || origin == UI::TOP_RIGHT;
                        //TODO: Still need to implement this
                        bool originLeft = origin == UI::TOP_LEFT || origin == UI::BOTTOM_LEFT;

                        float xOffset_l = 0;
                        float xOffset_r = 0;
                        float yOffset_t = originTop ? -(fontHeight - h) : h;
                        float yOffset_b = originTop ? -fontHeight : 0;
                        // update VBO for each character
                        float vertices[4][8] = {
                                {(xpos + xOffset_l),     (ypos + yOffset_t), zpos, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f},
                                {(xpos + xOffset_l),     (ypos + yOffset_b), zpos, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f},
                                {(xpos + xOffset_r + w), (ypos + yOffset_b), zpos, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f},
                                {(xpos + xOffset_r + w), (ypos + yOffset_t), zpos, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f}
                        };
                        // render glyph texture over quad
                        tchar.image.use(0);
                        // update content of VBO memory
                        glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
                        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
                        glBindBuffer(GL_ARRAY_BUFFER, 0);

                        if (mesh.EBO != 0)
                        {
                            //                           v-- number of indices to draw
                            glDrawElements(GL_TRIANGLES, mesh.drawCount, GL_UNSIGNED_INT,
                                           0); // NOLINT(modernize-use-nullptr)
                        }
                        else
                        {
                            //                            v-- number of vertices to draw
                            glDrawArrays(GL_TRIANGLES, 0, mesh.drawCount);
                        }
                        tchar.image.unuse(0);
                        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
                        x += (tchar.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
                    }
                }

                shader.unuse();
            }
        }
        else
        {
            LOGGER_ERROR("Failed to load glyph shader");
        }
    }
}