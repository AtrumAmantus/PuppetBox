#include "../UIComponents.h"

namespace PB
{
    void GroupComponent::update(float deltaTime)
    {
        PB::UI::Layout layoutDir = (PB::UI::Layout) UIComponent::getUIntAttribute(PB::UI::Attribute::LAYOUT)
                .orElse(PB::UI::Layout::HORIZONTAL);

        struct {
            uivec3 position{};
            std::uint32_t origin = 0;
        } group;

        group.position.x = UIComponent::getUIntAttribute(UI::POS_X).orElse(0);
        group.position.y = UIComponent::getUIntAttribute(UI::POS_Y).orElse(0);
        group.position.z = UIComponent::getUIntAttribute(UI::POS_Z).orElse(0);

        group.origin = UIComponent::getUIntAttribute(UI::ORIGIN).orElse(0);

        uivec2 offsets {
            group.position.x,
            group.position.y
        };

        for (auto& c : components_)
        {
            c->setUIntAttribute(UI::ORIGIN, UI::Origin::TOP_LEFT);
            c->setUIntAttribute(UI::POS_X, offsets.x);
            c->setUIntAttribute(UI::POS_Y, offsets.y);
            std::uint32_t childWidth = c->getUIntAttribute(UI::WIDTH).orElse(0);
            std::uint32_t childHeight = c->getUIntAttribute(UI::HEIGHT).orElse(0);
            offsets.x += (childWidth - (childWidth * layoutDir));
            offsets.y += (childHeight * layoutDir);
            c->update(deltaTime);
        }
    }

    void GroupComponent::render() const
    {

    }

    bool GroupComponent::addComponent(std::unique_ptr<UIComponent> component)
    {
        components_.push_back(component);
    }
}