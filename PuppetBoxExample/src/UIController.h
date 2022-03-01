#pragma once

#include <algorithm>
#include <memory>
#include <unordered_map>
#include <vector>

#include <puppetbox/UIComponent.h>

namespace
{
    bool compareZLevel(const std::shared_ptr<PB::UIComponent>& comp1, const std::shared_ptr<PB::UIComponent>& comp2)
    {
        return comp1->getUIntAttribute(PB::UI::Attribute::POS_Z).orElse(1)
               > comp2->getUIntAttribute(PB::UI::Attribute::POS_Z).orElse(0);
    }
}

class UIController
{
public:
    void addComponent(std::shared_ptr<PB::UIComponent> component, std::uint32_t referenceId)
    {
        componentList_.push_back(component);
        componentMap_.insert(
                std::pair<std::uint32_t, std::uint32_t>{referenceId, (std::uint32_t) componentList_.size() - 1}
        );
        std::sort(componentList_.begin(), componentList_.end(), compareZLevel);
    };

    std::shared_ptr<PB::UIComponent> getComponent(std::uint32_t referenceId, bool* error)
    {
        std::shared_ptr<PB::UIComponent> component;

        if (componentMap_.find(referenceId) != componentMap_.end())
        {
            component = componentList_.at(componentMap_.at(referenceId));
        }
        else
        {
            *error = true;
            component = nullptr;
        }

        return component;
    };

    void clear()
    {
        componentMap_.clear();
        componentList_.clear();
    };

    void update(float deltaTime)
    {
        for (auto component : componentList_)
        {
            component->update(deltaTime);
        }
    }

    void render() const
    {
        for (auto component : componentList_)
        {
            component->render();
        }
    };
private:
    std::unordered_map<std::uint32_t, std::uint32_t> componentMap_{};
    std::vector<std::shared_ptr<PB::UIComponent>> componentList_{};
};