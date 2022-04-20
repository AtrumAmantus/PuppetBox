#include "../UIComponents.h"

namespace PB
{
    TextAreaComponent::TextAreaComponent(
            UUID uuid,
            const std::shared_ptr<AssetLibrary>& assetLibrary,
            const std::shared_ptr<IGfxApi>& gfxApi
    ) : GfxUIComponent(uuid, assetLibrary, gfxApi)
    {

    }

    bool TextAreaComponent::inits()
    {
        return true;
    }

    void TextAreaComponent::update(float deltaTime)
    {

    }

    void TextAreaComponent::render() const
    {

    }

    void TextAreaComponent::updateStringAttribute(UI::Attribute::String attribute, std::string value)
    {
#ifdef _DEBUG
        LOGGER_DEBUG("Value set to '" + value + "'");
#endif
        switch (attribute)
        {
            case UI::Attribute::FONT_TYPE:
                break;
            case UI::Attribute::TEXT_CONTENT:
                break;
            default:
        }
    }

    void TextAreaComponent::updateUIntegerAttribute(UI::Attribute::UInteger attribute, std::uint32_t value)
    {
#ifdef _DEBUG
        LOGGER_DEBUG("Value set to '" + std::to_string(value) + "'");
#endif
        switch (attribute)
        {
            case UI::Attribute::FONT_SIZE:
                break;
            case UI::Attribute::HEIGHT:
                break;
            case UI::Attribute::ORIGIN:
                break;
            case UI::Attribute::POS_X:
                break;
            case UI::Attribute::POS_Y:
                break;
            case UI::Attribute::POS_Z:
                break;
            case UI::Attribute::WIDTH:
                break;
            default:
        }
    }
}