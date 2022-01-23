#pragma once

#include <memory>

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

        void update(float deltaTime) override;

        void render() const override;

    private:
        Font font_;
    };
}