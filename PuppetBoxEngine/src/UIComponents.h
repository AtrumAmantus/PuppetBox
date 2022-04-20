#pragma once

#include <memory>
#include <vector>

#include "puppetbox/DataStructures.h"
#include "AssetLibrary.h"
#include "GfxUIComponent.h"
#include "IGfxApi.h"

/**
 * UI is designed with an atomic design approach, with each {\link UIComponent} implementation representing
 * one small feature of a larger complex interface.
 */
namespace PB
{
    /**
     * Container to store individual glyph placement data calculated in
     * the Update phase, and used in the Render phase.
     */
    struct Glyph
    {
        ImageReference image{0};
        vec3 position{};
        uivec2 atlasPosition{};
        uivec2 originalDimensions{};
        vec2 scaledDimensions{};
        ivec2 charOffsets{};
        std::uint32_t advance = 0;
        std::int8_t character = '\0';
    };

    /**
     * Basic component for rendering text.
     *
     * Accepted attributes:
     *
     * {\see PB::UI::Attribute::UInteger::FONT_SIZE}
     * {\see PB::UI::Attribute::String::FONT_TYPE}
     * {\see PB::UI::Attribute::UInteger::HEIGHT}
     * {\see PB::UI::Attribute::UInteger::ORIGIN}
     * {\see PB::UI::Attribute::UInteger::POS_X}
     * {\see PB::UI::Attribute::UInteger::POS_Y}
     * {\see PB::UI::Attribute::UInteger::POS_Z}
     * {\see PB::UI::Attribute::String::TEXT_CONTENT}
     * {\see PB::UI::Attribute::UInteger::WIDTH}
     */
    class TextAreaComponent : public GfxUIComponent
    {
    public:
        TextAreaComponent(
                UUID uuid,
                const std::shared_ptr<AssetLibrary>& assetLibrary,
                const std::shared_ptr<IGfxApi>& gfxApi);

        void update(float deltaTime) override;

        void render() const override;

    protected:
        bool inits() override;

        void updateStringAttribute(UI::Attribute::String attribute, std::string value) override;

        void updateUIntegerAttribute(UI::Attribute::UInteger attribute, std::uint32_t value) override;

    private:
        Shader shader_{""};
        std::uint32_t VAO_;
        std::uint32_t VBO_;
        std::vector<Glyph> glyphs{};
        Font font_{};
    };

    class GroupComponent : public GfxUIComponent
    {
    public:
        GroupComponent(
                UUID uuid,
                const std::shared_ptr<AssetLibrary>& assetLibrary,
                const std::shared_ptr<IGfxApi>& gfxApi);

        void update(float deltaTime) override;

        void render() const override;

        bool addComponent(UUID uuid) override;

    private:
        std::vector<std::shared_ptr<UIComponent>> components_;
    };
}