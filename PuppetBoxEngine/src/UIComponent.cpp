#include <unordered_map>

#include "puppetbox/UIComponent.h"
#include "Logger.h"

namespace PB
{
    /**
     * Basic implementation of the {\link UIComponentAttributes}, storing attributes in various
     * {\link unordered_map} types.
     *
     * <p>Implementation definition is hidden outside this file</p>
     */
    class SimpleUIComponentAttributes : public UIComponentAttributes
    {
    public:
        void setUIntAttribute(UI::Attribute attributeName, std::uint32_t value) override
        {
            uintMap_[attributeName] = value;
        };

        Result<std::uint32_t> getUIntAttribute(UI::Attribute attributeName) const override
        {
            Result<std::uint32_t> result{};

            if (uintMap_.find(attributeName) != uintMap_.end())
            {
                result.hasResult = true;
                result.result = uintMap_.at(attributeName);
            }
            else
            {
                result.hasResult = false;
            }

            return result;
        };

        void setIntAttribute(UI::Attribute attributeName, std::int32_t value) override
        {
            intMap_[attributeName] = value;
        };

        Result<std::int32_t> getIntAttribute(UI::Attribute attributeName) const override
        {
            Result<std::int32_t> result{};

            if (intMap_.find(attributeName) != intMap_.end())
            {
                result.hasResult = true;
                result.result = intMap_.at(attributeName);
            }
            else
            {
                result.hasResult = false;
            }

            return result;
        };

        void setStringAttribute(UI::Attribute attributeName, const std::string& value) override
        {
            stringMap_[attributeName] = value;
        };

        Result<std::string> getStringAttribute(UI::Attribute attributeName) const override
        {
            Result<std::string> result{};

            if (stringMap_.find(attributeName) != stringMap_.end())
            {
                result.hasResult = true;
                result.result = stringMap_.at(attributeName);
            }
            else
            {
                result.hasResult = false;
            }

            return result;
        };

    private:
        std::unordered_map<UI::Attribute, std::int32_t> intMap_{};
        std::unordered_map<UI::Attribute, std::uint32_t> uintMap_{};
        std::unordered_map<UI::Attribute, std::string> stringMap_{};
    };

    UIComponent::UIComponent()
    {
        attributes_ = std::unique_ptr<SimpleUIComponentAttributes>();
    }

    std::unique_ptr<UIComponentAttributes> UIComponent::createUIComponentAttributes()
    {
        return std::make_unique<SimpleUIComponentAttributes>();
    }

    bool UIComponent::init()
    {
        return true;
    }

    bool UIComponent::addComponent(std::unique_ptr<UIComponent> component)
    {
        LOGGER_ERROR("addComponent not implemented for this UI Component.");
        return false;
    }

    void UIComponent::setAttributes(std::unique_ptr<UIComponentAttributes> attributes)
    {
        attributes_ = std::move(attributes);
    }

    void UIComponent::setUIntAttribute(UI::Attribute attributeName, std::uint32_t value)
    {
        attributes_->setUIntAttribute(attributeName, value);
    }

    Result<std::uint32_t> UIComponent::getUIntAttribute(UI::Attribute attributeName) const
    {
        return attributes_->getUIntAttribute(attributeName);
    }

    void UIComponent::setIntAttribute(UI::Attribute attributeName, std::int32_t value)
    {
        attributes_->setIntAttribute(attributeName, value);
    }

    Result<std::int32_t> UIComponent::getIntAttribute(UI::Attribute attributeName) const
    {
        return attributes_->getIntAttribute(attributeName);
    }

    void UIComponent::setStringAttribute(UI::Attribute attributeName, const std::string& value)
    {
        attributes_->setStringAttribute(attributeName, value);
    }

    Result<std::string> UIComponent::getStringAttribute(UI::Attribute attributeName) const
    {
        return attributes_->getStringAttribute(attributeName);
    }
}