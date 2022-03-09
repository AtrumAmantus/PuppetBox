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

        if (error)
        {
            LOGGER_ERROR("Failed to load shader asset 'Assets1/Shaders/UI/Glyph'");
        }

        // Setting both Vertex and UV origin to 0 to make shader calculations simpler
        //TODO: This may need to be revisited, offset origin could get weird later
        float vertices[] = {
                // Vertex                           // UV
                0.0f, 1.0f, 0.0f,       0.0f, 1.0f, // Top left
                0.0f, 0.0f, 0.0f,       0.0f, 0.0f, // Bot left
                1.0f, 0.0f, 0.0f,       1.0f, 0.0f, // Bot right

                0.0f, 1.0f, 0.0f,       0.0f, 1.0f, // Top left
                1.0f, 0.0f, 0.0f,       1.0f, 0.0f, // Bot right
                1.0f, 1.0f, 0.0f,       1.0f, 1.0f  // Top right
        };

        glGenVertexArrays(1, &VAO_);
        glGenBuffers(1, &VBO_);

        glBindVertexArray(VAO_);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

        glBindVertexArray(0);

        return !error;
    }

    void TextAreaComponent::update(float deltaTime)
    {
        //TODO: Still need to handle word breaks and vertical align on text.
        bool error = false;

        glyphs.clear();

        std::string text = UIComponent::getStringAttribute(UI::TEXT_CONTENT).orElse("");

        std::vector<std::string> paragraphs{};
        std::uint32_t lineStart = 0;
        std::uint32_t lineEnd = 0;

        while (lineEnd < text.size())
        {
            if (text.c_str()[lineEnd] == '\n')
            {
                paragraphs.push_back(text.substr(lineStart, lineEnd - lineStart));
                lineStart = lineEnd + 1;
            }

            ++lineEnd;
        }

        if (lineStart != lineEnd)
        {
            paragraphs.push_back(text.substr(lineStart, lineEnd - lineStart));
        }

        if (!text.empty())
        {
            Result<std::string> fontName = UIComponent::getStringAttribute(UI::FONT_TYPE);

            //TODO: Load default font if this fails
            if (fontName.hasResult)
            {
                font_ = library()->loadFontAsset(fontName.result, 0, &error);

                struct
                {
                    uivec3 position;
                    uivec2 dimensions;
                    std::string fontPath;
                    std::uint32_t fontSize;
                    bool wordWrapEnabled;
                    float letterSpacing;
                    float wordSpacing;
                } component;

                component.position.x = UIComponent::getUIntAttribute(UI::POS_X).orElse(0);
                component.position.y = UIComponent::getUIntAttribute(UI::POS_Y).orElse(0);
                component.position.z = UIComponent::getUIntAttribute(UI::POS_Z).orElse(0);
                component.dimensions.x = UIComponent::getUIntAttribute(UI::WIDTH).orElse(0);
                component.dimensions.y = UIComponent::getUIntAttribute(UI::HEIGHT).orElse(0);
                component.fontSize = UIComponent::getUIntAttribute(UI::FONT_SIZE).orElse(0);
                component.fontPath = UIComponent::getStringAttribute(UI::FONT_TYPE).orElse("");
                component.letterSpacing = UIComponent::getFloatAttribute(UI::LETTER_SPACE).orElse(1.0f);
                component.wordSpacing = UIComponent::getFloatAttribute(UI::WORD_SPACE).orElse(1.0f);
                component.wordWrapEnabled = UIComponent::getBoolAttribute(UI::WORD_WRAP).orElse(true);

                float scale = (float) component.fontSize / font_.fontSize();

                std::string::const_iterator c;
                vec3 localPosition{};
                vec3 paragraphPosition{};

                float deltaY = component.fontSize + 2;

                float wordFirstLetterOffset = 0;

                std::uint32_t firstCharOfWordIndex = 0;
                std::uint32_t firstCharOfParagraphIndex = 0;

                std::int32_t paragraphIndex = paragraphs.size();

                // For each paragraph
                while (-(localPosition.y - deltaY) < component.dimensions.y && --paragraphIndex >= 0)
                {
                    c = paragraphs.at(paragraphIndex).begin();
                    firstCharOfParagraphIndex = glyphs.size();
                    paragraphPosition.y = 0;

                    // For each character in a paragraph
                    // localPosition.y represents the "top", so we decrement it as we go.
                    while (
                            -(localPosition.y - deltaY) <= component.dimensions.y
                            && c != paragraphs.at(paragraphIndex).end())
                    {
                        // TODO: Add a default character to load for unrecognized ones
                        TypeCharacter tchar = font_.getCharacter(*c).result;

                        Glyph glyph;

                        glyph.charOffsets.x = tchar.bearing.x * scale;
                        glyph.charOffsets.y = -((tchar.size.y - tchar.bearing.y) * scale);

                        glyph.position.x = localPosition.x + glyph.charOffsets.x;
                        glyph.position.y = paragraphPosition.y + glyph.charOffsets.y;
                        glyph.position.z = localPosition.z;

                        glyph.originalDimensions = tchar.size;

                        glyph.scaledDimensions.x = tchar.size.x * scale;
                        glyph.scaledDimensions.y = tchar.size.y * scale;

                        glyph.atlasPosition = tchar.atlasPosition;
                        glyph.image = tchar.image;

                        glyph.advance = tchar.advance;

                        glyph.character = *c;

                        // Check for space sto identify the start of a new word
                        if (*c == ' ')
                        {
                            //TODO: Word spacing isn't quite working
                            glyph.advance = tchar.advance * component.wordSpacing;
                            // First character of next word will be the next one after this character, so
                            // size + 1 for next index + 1
                            firstCharOfWordIndex = glyphs.size() + 1;
                        }

                        // Check for horizontal clipping
                        if ((localPosition.x + glyph.scaledDimensions.x) > component.dimensions.x
                            && firstCharOfWordIndex < glyphs.size())
                        {
                            // If the letter is clipped (and not a space), move the whole word down a line
                            // if wordwrap is enabled, otherwise it gets thrown out
                            if (component.wordWrapEnabled)
                            {
                                Glyph firstCharOfWordGlyph = glyphs.at(firstCharOfWordIndex);
                                // Remove character offset to get true x coord of first char in word
                                wordFirstLetterOffset = firstCharOfWordGlyph.position.x
                                                        - firstCharOfWordGlyph.charOffsets.x;

                                //TODO: Is this threshold too high?
                                if (wordFirstLetterOffset > 0.01)
                                {
                                    localPosition.x = 0;
                                    paragraphPosition.y -= deltaY;

                                    // Move every character of the word down
                                    while (firstCharOfWordIndex < glyphs.size())
                                    {
                                        Glyph& storedGlyph = glyphs.at(firstCharOfWordIndex++);
                                        storedGlyph.position.x = localPosition.x + storedGlyph.charOffsets.x;
                                        storedGlyph.position.y = paragraphPosition.y + storedGlyph.charOffsets.y;
                                        localPosition.x += ((storedGlyph.advance >> 6) * component.letterSpacing *
                                                            scale);
                                    }

                                    // Update current character as well
                                    glyph.position.x = localPosition.x + glyph.charOffsets.x;
                                    glyph.position.y = paragraphPosition.y + glyph.charOffsets.y;
                                    glyphs.push_back(glyph);
                                }
                            }
                        }
                        else
                        {
                            glyphs.push_back(glyph);
                        }

                        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
                        // bitshift by 6 to get value in pixels (2^6 = 64)
                        localPosition.x +=
                                (tchar.advance >> 6) * component.letterSpacing * scale;

                        ++c;
                    }

                    std::uint32_t paragraphToTopDelta;
                    std::uint32_t paragraphShift;

                    // Move current paragraph above previous paragraph
                    for (std::uint32_t i = firstCharOfParagraphIndex; i < glyphs.size(); ++i)
                    {
                        auto& glyph = glyphs.at(i);
                        // Difference between top of frame and top of previous paragraph
                        // This is likely a negative value since y = 0 is the top of the frame
                        // Because localPosition.y is decremented, the sum gives the delta
                        // As localPosition.y becomes more negative, this delta gets smaller
                        paragraphToTopDelta = component.dimensions.y + localPosition.y;
                        // Distance of the top of the current glyph to the bottom of the paragraph
                        // This creates a positive value, shifting [0, -1, -2, ...] to [..., 2, 1, 0]
                        // The calculation is based on the top of the glyph, so shift everything up one extra line,
                        // making the "bottom" of the glyphs in the last line of the paragraph the origin.
                        paragraphShift = (-paragraphPosition.y) + deltaY;
                        // Shift the paragraph "up" so the bottom of the paragraph is at y = 0
                        glyph.position.y += paragraphShift;
                        // Shift the paragraph "down" so the bottom of the paragraph is at y = top of previous paragraph
                        glyph.position.y -= paragraphToTopDelta;
                    }

                    localPosition.x = 0;
                    // New localPosition.y to track previous paragraph position
                    localPosition.y += paragraphPosition.y - deltaY;
                    paragraphPosition.y = 0;
                }
            }
        }
    }

    void TextAreaComponent::render() const
    {
        //TODO: Abstract the OpenGL calls/logic away

        struct
        {
            UI::Origin origin;
            uivec3 position;
            uivec2 dimension;
        } component;

        component.origin = (UI::Origin) UIComponent::getUIntAttribute(UI::ORIGIN).orElse(UI::Origin::BOTTOM_LEFT);

        component.position.x = UIComponent::getUIntAttribute(UI::POS_X).orElse(0);
        component.position.y = UIComponent::getUIntAttribute(UI::POS_Y).orElse(0);
        component.position.z = UIComponent::getUIntAttribute(UI::POS_Z).orElse(0);

        component.dimension.x = UIComponent::getUIntAttribute(UI::WIDTH).orElse(0);
        component.dimension.y = UIComponent::getUIntAttribute(UI::HEIGHT).orElse(0);

        std::int32_t fontHeight = UIComponent::getUIntAttribute(UI::FONT_SIZE).orElse(0);

        shader_.use();
        shader_.setVec3("textColour", vec3{1.0f, 1.0f, 1.0f});

        glBindVertexArray(VAO_);

        const std::uint32_t MAX_INSTANCE_COUNT = 100;
        const std::uint32_t MAX_INSTANCE_SIZE = MAX_INSTANCE_COUNT * 9;

        auto itr = glyphs.begin();
        float instanceData[MAX_INSTANCE_SIZE];
        std::uint32_t instanceCount = 0;

        TypeCharacter c = font_.getCharacter(' ').result;
        c.image.use(0);

        std::uint32_t instanceDataIndex = 0;

        // Build sets of 100 glyphs, then render them as an instance group
        while (itr != glyphs.end())
        {
            if (instanceCount >= MAX_INSTANCE_COUNT)
            {
                shader_.setFloatArray("instanceData", instanceDataIndex, instanceData);

                glDrawArraysInstanced(GL_TRIANGLES, 0, 6, instanceCount);

                instanceCount = 0;
                instanceDataIndex = 0;
            }

            bool originTop = component.origin == UI::TOP_LEFT || component.origin == UI::TOP_RIGHT;
            //TODO: Still need to implement this
            bool originRight = component.origin == UI::TOP_RIGHT || component.origin == UI::BOTTOM_RIGHT;

            vec3 containerOffset{};
            containerOffset.x = component.position.x - (component.dimension.x * originRight);
            containerOffset.y = component.position.y
                                // Raises up if origin is bottom
                                // NOTE: Glyph vertices are drawn with origin in bottom left, so adjust for font height
                                // when raising up.
                                + (component.dimension.y - fontHeight)
                                // Brings back down if origin is top
                                - (component.dimension.y * originTop);

            instanceData[instanceDataIndex++] = itr->position.x + containerOffset.x;
            instanceData[instanceDataIndex++] = itr->position.y + containerOffset.y;// + itr->scaledDimensions.y;
            instanceData[instanceDataIndex++] = itr->position.z;
            instanceData[instanceDataIndex++] = itr->scaledDimensions.x;
            instanceData[instanceDataIndex++] = itr->scaledDimensions.y;
            instanceData[instanceDataIndex++] = static_cast<float>(itr->atlasPosition.x) / itr->image.width;
            instanceData[instanceDataIndex++] = static_cast<float>(itr->atlasPosition.y) / itr->image.height;
            instanceData[instanceDataIndex++] = static_cast<float>(itr->originalDimensions.x) / itr->image.width;
            instanceData[instanceDataIndex++] = static_cast<float>(itr->originalDimensions.y) / itr->image.height;

            ++itr;
            ++instanceCount;
        }

        // Render any remaining glyphs
        if (instanceCount > 0)
        {
            std::int32_t val = 0;
            glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &val);
            shader_.setFloatArray("instanceData", instanceDataIndex, instanceData);

            glDrawArraysInstanced(GL_TRIANGLES, 0, 6, instanceCount);

            instanceCount = 0;
            instanceDataIndex = 0;
        }

        glBindVertexArray(0);

        c.image.unuse(0);

        shader_.unuse();
    }
}