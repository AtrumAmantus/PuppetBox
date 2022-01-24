#pragma once

#include <PuppetBox.h>

class UIAttributeBuilder
{
public:
    UIAttributeBuilder& origin(PB::UI::Origin origin)
    {
        origin_.result = origin;
        origin_.hasResult = true;
        return *this;
    }

    UIAttributeBuilder& position(PB::vec3 position)
    {
        position_.result = position;
        position_.hasResult = true;
        return *this;
    }

    UIAttributeBuilder& dimensions(PB::vec2 dimensions)
    {
        dimensions_.result = dimensions;
        dimensions_.hasResult = true;
        return *this;
    }

    UIAttributeBuilder& fontSize(std::uint32_t fontSize)
    {
        fontSize_.result = fontSize;
        fontSize_.hasResult = true;
        return *this;
    }

    UIAttributeBuilder& fontType(std::string fontType)
    {
        fontType_.result = fontType;
        fontType_.hasResult = true;
        return *this;
    }

    std::unique_ptr<PB::UIComponentAttributes> build()
    {
        std::unique_ptr<PB::UIComponentAttributes> attributes = PB::UIComponent::createUIComponentAttributes();

        if (origin_.hasResult)
        {
            attributes->setUIntAttribute(PB::UI::ORIGIN, origin_.result);
        }

        if (position_.hasResult)
        {
            attributes->setUIntAttribute(PB::UI::POS_X, position_.result.x);
            attributes->setUIntAttribute(PB::UI::POS_Y, position_.result.y);
            attributes->setUIntAttribute(PB::UI::POS_Z, position_.result.z);
        }

        if (dimensions_.hasResult)
        {
            attributes->setUIntAttribute(PB::UI::WIDTH, dimensions_.result.x);
            attributes->setUIntAttribute(PB::UI::HEIGHT, dimensions_.result.y);
        }

        if (fontSize_.hasResult)
        {
            attributes->setUIntAttribute(PB::UI::FONT_SIZE, fontSize_.result);
        }

        if (fontType_.hasResult)
        {
            attributes->setStringAttribute(PB::UI::FONT_TYPE, fontType_.result);
        }

        if (textContent_.hasResult)
        {
            attributes->setStringAttribute(PB::UI::TEXT_CONTENT, textContent_.result);
        }

        return std::move(attributes);
    }
private:
    PB::Result<std::uint32_t> origin_{0, false};
    PB::Result<PB::vec3> position_{{}, false};
    PB::Result<PB::vec2> dimensions_{{}, false};
    PB::Result<std::uint32_t> fontSize_{0, false};
    PB::Result<std::string> fontType_{"", false};
    PB::Result<std::string> textContent_{"", false};
};