#include "../UIComponents.h"

namespace PB
{
    GroupComponent::GroupComponent(
            const std::shared_ptr<AssetLibrary>& assetLibrary,
            const std::shared_ptr<IGfxApi>& gfxApi
    ) : GfxUIComponent(assetLibrary, gfxApi)
    {

    }

    void GroupComponent::update(float deltaTime)
    {
        PB::UI::Layout layoutDir = (PB::UI::Layout) UIComponent::getUIntAttribute(PB::UI::Attribute::LAYOUT)
                .orElse(PB::UI::Layout::HORIZONTAL);

        struct
        {
            ivec3 position;
            UI::Origin origin;
        } component;

        component.position.x = UIComponent::getUIntAttribute(UI::POS_X).orElse(0);
        component.position.y = UIComponent::getUIntAttribute(UI::POS_Y).orElse(0);
        component.position.z = UIComponent::getUIntAttribute(UI::POS_Z).orElse(0);

        component.origin = (UI::Origin) UIComponent::getUIntAttribute(UI::ORIGIN).orElse(0);

        ivec2 offsets{
                component.position.x,
                component.position.y
        };

        bool originBottom = component.origin == UI::Origin::BOTTOM_LEFT || component.origin == UI::Origin::BOTTOM_RIGHT;
        bool originRight = component.origin == UI::Origin::TOP_RIGHT || component.origin == UI::Origin::BOTTOM_RIGHT;

        //TODO: Remove branches
        std::int32_t inc;
        std::int32_t start;

        if ((layoutDir == UI::HORIZONTAL && originRight) || (layoutDir == UI::VERTICAL && originBottom))
        {
            inc = -1;
            start = components_.size() - 1;
        }
        else
        {
            inc = 1;
            start = 0;
        }

        // Iterate forwards or backwards depending on layoutDir
        for (std::int32_t i = start; (i * inc) < (std::int32_t)(components_.size() - start); i += inc)
        {
            // Set child's absolute location using parent's data
            components_.at(i)->setUIntAttribute(UI::ORIGIN, component.origin);
            components_.at(i)->setUIntAttribute(UI::POS_X, offsets.x);
            components_.at(i)->setUIntAttribute(UI::POS_Y, offsets.y);

            // Calculate next child's location
            std::int32_t childWidth = components_.at(i)->getUIntAttribute(UI::WIDTH).orElse(0);
            std::int32_t childHeight = components_.at(i)->getUIntAttribute(UI::HEIGHT).orElse(0);
            offsets.x += ((childWidth - (childWidth * layoutDir)) - (2 * (childWidth - (childWidth * layoutDir)) * originRight));
            offsets.y += (-(childHeight * layoutDir)) + (2 * (childHeight * layoutDir) * originBottom);

            // Execute child's update logic
            components_.at(i)->update(deltaTime);
        }
    }

    void GroupComponent::render() const
    {
        for (auto& c: components_)
        {
            c->render();
        }
    }

    bool GroupComponent::addComponent(std::shared_ptr<UIComponent> component)
    {
        components_.push_back(component);
        return true;
    }
}