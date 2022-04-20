#pragma once

#include <memory>
#include <utility>

#include "puppetbox/UIComponent.h"

#include "AssetLibrary.h"
#include "IGfxApi.h"

namespace PB
{
    /**
     * Contains GFX specific resources for UI components.
     *
     * <p>Access to the {\link AssetLibrary} for pulling fonts, etc.</p>
     *
     * <p>Access to the {\link IGfxApi} for rendering the component.</p>
     */
    class GfxUIComponent : public UIComponent
    {
    public:
        GfxUIComponent(UUID uuid, std::shared_ptr<AssetLibrary> assetLibrary, std::shared_ptr<IGfxApi> gfxApi)
                : UIComponent(uuid), assetLibrary_(std::move(assetLibrary)), gfxApi_(std::move(gfxApi)) {};

    protected:
        /**
         * \brief Shared reference to the {\link AssetLibrary} instance for rendering UI components.
         *
         * \return Reference to the shared {\link AssetLibrary} instance.
         */
        const std::shared_ptr<AssetLibrary>& library() const
        {
            return assetLibrary_;
        };

        /**
         * \brief Shared reference to the {\link IGfxApi} instance for GFX specific calls.
         *
         * \return Reference to the shared {\link IGfxApi} instance.
         */
        const std::shared_ptr<IGfxApi>& gfxApi()
        {
            return gfxApi_;
        };
    private:
        std::shared_ptr<AssetLibrary> assetLibrary_;
        std::shared_ptr<IGfxApi> gfxApi_;
    };
}