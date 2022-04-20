#include <memory>

#include "puppetbox/UIComponent.h"
#include "../EventDef.h"
#include "../Logger.h"
#include "../MessageBroker.h"

namespace PB
{
    UIComponent::UIComponent(UUID id) : id_(id)
    {

    }

    UIComponent::~UIComponent()
    {
        for (auto subscription : subscriptions_)
        {
            MessageBroker::instance().unsubscribe(subscription);
        }
    }

    bool UIComponent::init()
    {
        auto uuid = MessageBroker::instance().subscribe(
                PB_EVENT_UI_SET_STRING_ATTRIBUTE "_" + std::to_string(id_),
                [this](std::shared_ptr<void> data) {
                    auto event = std::static_pointer_cast<UIUpdateStringEvent>(data);

                    updateStringAttribute(event->attribute, event->value);
                });

        subscriptions_.push_back(uuid);

        uuid = MessageBroker::instance().subscribe(
                PB_EVENT_UI_SET_UINT_ATTRIBUTE "_" + std::to_string(id_),
                [this](std::shared_ptr<void> data) {
                    auto event = std::static_pointer_cast<UIUpdateUIntEvent>(data);

                    updateUIntegerAttribute(event->attribute, event->value);
                });

        subscriptions_.push_back(uuid);

        uuid = MessageBroker::instance().subscribe(
                PB_EVENT_UI_SET_FLOAT_ATTRIBUTE "_" + std::to_string(id_),
                [this](std::shared_ptr<void> data) {
                    auto event = std::static_pointer_cast<UIUpdateFloatEvent>(data);

                    updateFloatAttribute(event->attribute, event->value);
                });

        subscriptions_.push_back(uuid);

        uuid = MessageBroker::instance().subscribe(
                PB_EVENT_UI_SET_BOOL_ATTRIBUTE "_" + std::to_string(id_),
                [this](std::shared_ptr<void> data) {
                    auto event = std::static_pointer_cast<UIUpdateBoolEvent>(data);

                    updateBooleanAttribute(event->attribute, event->value);
                });

        subscriptions_.push_back(uuid);

        return inits();
    }

    bool UIComponent::addComponent(UUID uuid)
    {
        LOGGER_ERROR("addComponent not implemented for this UI Component.");
        return false;
    }

    bool UIComponent::inits()
    {
        return true;
    }

    void UIComponent::updateStringAttribute(UI::Attribute::String attribute, std::string value)
    {

    }

    void UIComponent::updateUIntegerAttribute(UI::Attribute::UInteger attribute, std::uint32_t value)
    {

    }

    void UIComponent::updateFloatAttribute(UI::Attribute::Float attribute, float value)
    {

    }

    void UIComponent::updateBooleanAttribute(UI::Attribute::Boolean attribute, bool value)
    {

    }
}