#include "../UIComponents.h"

namespace PB
{
    GroupComponent::GroupComponent(
            UUID uuid,
            const std::shared_ptr<AssetLibrary>& assetLibrary,
            const std::shared_ptr<IGfxApi>& gfxApi
    ) : GfxUIComponent(uuid, assetLibrary, gfxApi)
    {

    }

    void GroupComponent::update(float deltaTime)
    {

    }

    void GroupComponent::render() const
    {

    }

    bool GroupComponent::addComponent(UUID uuid)
    {
        return true;
    }
}