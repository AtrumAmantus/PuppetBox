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
        vec3 position{};
        vec2 dimensions{};
        ivec2 charOffsets{};
        std::uint32_t advance = 0;
        std::int8_t character = '\0';
    };

    /**
     * Basic component for rendering text.
     *
     * Accepted attributes:
     *
     * FONT_SIZE
     * FONT_TYPE
     * HEIGHT
     * ORIGIN
     * POS_X
     * POS_Y
     * POS_Z
     * TEXT_CONTENT
     * WIDTH
     */
    class TextAreaComponent : public GfxUIComponent
    {
    public:
        TextAreaComponent(const std::shared_ptr<AssetLibrary>& assetLibrary, const std::shared_ptr<IGfxApi>& gfxApi);

        bool init() override;

        void update(float deltaTime) override;

        void render() const override;

    private:
        Shader shader_{""};
        std::vector<Glyph> glyphs{};
        Font font_{};
    };

    class GroupComponent : public GfxUIComponent
    {
    public:
        GroupComponent(const std::shared_ptr<AssetLibrary>& assetLibrary, const std::shared_ptr<IGfxApi>& gfxApi);

        void update(float deltaTime) override;

        void render() const override;

        bool addComponent(std::shared_ptr<UIComponent> component) override;

    private:
        std::vector<std::shared_ptr<UIComponent>> components_;
    };
}