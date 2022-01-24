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

    bool TextAreaComponent::init()
    {
        bool error = false;
        shader_ = library()->loadShaderAsset("Assets1/Shaders/UI/Glyph", &error);
        return !error;
    }

    void TextAreaComponent::update(float deltaTime)
    {
        //TODO: Still need to handle word breaks and vertical align on text.
        bool error = false;

        glyphs.clear();

        std::string text = UIComponent::getStringAttribute(UI::TEXT_CONTENT).orElse("");

        if (!text.empty())
        {
            Result<std::string> fontName = UIComponent::getStringAttribute(UI::FONT_TYPE);

            //TODO: Load default font if this fails
            if (fontName.hasResult)
            {
                font_ = library()->loadFontAsset(fontName.result, 0, &error);

                vec3 componentPosition{};
                componentPosition.x = UIComponent::getUIntAttribute(UI::POS_X).orElse(0);
                componentPosition.y = UIComponent::getUIntAttribute(UI::POS_Y).orElse(0);
                componentPosition.z = UIComponent::getUIntAttribute(UI::POS_Z).orElse(0);
                std::int32_t fontSize = UIComponent::getUIntAttribute(UI::FONT_SIZE).orElse(0);
                std::string fontPath = UIComponent::getStringAttribute(UI::FONT_TYPE).orElse("");
                std::uint32_t origin = UIComponent::getUIntAttribute(UI::ORIGIN).orElse(UI::Origin::BOTTOM_LEFT);
                std::uint32_t componentWidth = UIComponent::getUIntAttribute(UI::WIDTH).orElse(0);
                std::uint32_t componentHeight = UIComponent::getUIntAttribute(UI::HEIGHT).orElse(0);
                float scale = (float) fontSize / font_.fontSize();

                std::string::const_iterator c;
                vec3 localPosition{};

                float deltaY = fontSize + 2;

                c = text.begin();
                // y increments "down", so invert before comparing
                while (-(localPosition.y + deltaY) < componentHeight && c != text.end())
                {
                    if (*c == '\n')
                    {
                        localPosition.x = 0;
                        localPosition.y -= deltaY;
                        ++c;
                    }
                    else
                    {
                        // TODO: Add a default character to load for unrecognized ones
                        TypeCharacter tchar = font_.getCharacter(*c).result;

                        Glyph glyph;

                        glyph.position.x = localPosition.x + tchar.bearing.x * scale;
                        glyph.position.y = localPosition.y - (tchar.size.y - tchar.bearing.y) * scale;
                        glyph.position.z = localPosition.z;

                        glyph.dimensions.x = tchar.size.x * scale;
                        glyph.dimensions.y = tchar.size.y * scale;

                        glyph.character = *c;

                        if (localPosition.x + glyph.dimensions.x > componentWidth)
                        {
                            localPosition.x = 0;
                            //TODO: Check if we're breaking a word and move it down if we are.
                            localPosition.y -= deltaY;
                        }
                        else
                        {
                            glyphs.push_back(glyph);

                            // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
                            localPosition.x +=
                                    (tchar.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)

                            ++c;
                        }
                    }
                }
            }
        }
    }

    void TextAreaComponent::render() const
    {
        //TODO: Abstract the OpenGL calls/logic away
        bool error = false;

        struct
        {
            std::uint32_t origin = 0;
            uivec3 position{};
            uivec2 dimension{};
        } component;

        component.origin = UIComponent::getUIntAttribute(UI::ORIGIN).orElse(UI::Origin::BOTTOM_LEFT);

        component.position.x = UIComponent::getUIntAttribute(UI::POS_X).orElse(0);
        component.position.y = UIComponent::getUIntAttribute(UI::POS_Y).orElse(0);
        component.position.z = UIComponent::getUIntAttribute(UI::POS_Z).orElse(0);

        component.dimension.x = UIComponent::getUIntAttribute(UI::WIDTH).orElse(0);
        component.dimension.y = UIComponent::getUIntAttribute(UI::HEIGHT).orElse(0);

        std::int32_t fontHeight = UIComponent::getUIntAttribute(UI::FONT_SIZE).orElse(0);

        shader_.use();
        shader_.setVec3("textColour", vec3{1.0f, 1.0f, 1.0f});

        Mesh mesh = library()->loadMeshAsset("Assets1/Mesh/Glyph", &error);
        glBindVertexArray(mesh.VAO);

        for (auto& g: glyphs)
        {
            bool originTop = component.origin == UI::TOP_LEFT || component.origin == UI::TOP_RIGHT;
            //TODO: Still need to implement this
            bool originRight = component.origin == UI::TOP_RIGHT || component.origin == UI::BOTTOM_RIGHT;

            vec3 containerOffset{};
            containerOffset.x = component.position.x - (component.dimension.x * originRight);
            containerOffset.y = component.position.y
                                + (component.dimension.y - fontHeight) // Raises up if origin is bottom
                                - (component.dimension.y * originTop); // Brings back down if origin is top

            //TODO: This may need to be reworked to support "floating" text in 3d space.
            float vertices[4][8] = {
                    {
                            g.position.x + containerOffset.x,
                            g.position.y + containerOffset.y + g.dimensions.y,
                            g.position.z,

                            0.0f, 0.0f, 1.0f,   // Normals don't change
                            0.0f, 1.0f          // UV coords don't change
                    },
                    {
                            g.position.x + containerOffset.x,
                            g.position.y + containerOffset.y,
                            g.position.z,

                            0.0f, 0.0f, 1.0f,
                            0.0f, 0.0f
                    },
                    {
                            g.position.x + containerOffset.x + g.dimensions.x,
                            g.position.y + containerOffset.y,
                            g.position.z,

                            0.0f, 0.0f, 1.0f,
                            1.0f, 0.0f
                    },
                    {
                            g.position.x + containerOffset.x + g.dimensions.x,
                            g.position.y + containerOffset.y + g.dimensions.y,
                            g.position.z,

                            0.0f, 0.0f, 1.0f,
                            1.0f, 1.0f
                    }
            };

            // TODO: Add a default character to load for unrecognized ones
            TypeCharacter tchar = font_.getCharacter(g.character).result;

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
        }

        shader_.unuse();
    }
}