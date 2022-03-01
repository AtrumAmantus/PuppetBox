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
        referenceComponent(component, referenceId);
        std::sort(componentList_.begin(), componentList_.end(), compareZLevel);
    };

    void referenceComponent(std::shared_ptr<PB::UIComponent>& component, std::uint32_t referenceId)
    {
        componentMap_.insert(
                std::pair<std::uint32_t, std::shared_ptr<PB::UIComponent>>{referenceId, component}
        );
    };

    std::shared_ptr<PB::UIComponent> getComponent(std::uint32_t referenceId, bool* error)
    {
        std::shared_ptr<PB::UIComponent> component;

        auto itr = componentMap_.find(referenceId);

        if (itr != componentMap_.end())
        {
            component = itr->second;
        }
        else
        {
            *error = true;
            component = nullptr;
        }

        return std::move(component);
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
    std::unordered_map<std::uint32_t, std::shared_ptr<PB::UIComponent>> componentMap_{};
    std::vector<std::shared_ptr<PB::UIComponent>> componentList_{};
};