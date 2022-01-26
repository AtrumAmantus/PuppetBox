#pragma once

#include <memory>
#include <string>

#include "DataStructures.h"
#include "TypeDef.h"

namespace PB
{
    namespace UI
    {
        /**
         * The various {\link UIComponent} types that can be made to build more complex user interfaces.
         */
        enum Type
        {
            /**
             * Defines a {\link UIComponent} that groups several components together.
             */
            GROUP,
            /**
             * Defines a {\link UIComponent} that renders text on the screen.
             */
            TEXT_AREA,
        };

        /**
         * Various attributes that can be associated to {\link UIComponent}s.
         *
         * <p>Attributes can be set to any of the types offered by the API and applied to any
         * UI component without throwing an error, but each attribute is only checked using one type,
         * and each {\link UIComponent} implementation only checks for certain attributes.<p>
         */
        enum Attribute
        {
            /**
             * \brief The size to render the component's text at.
             *
             * <p>Expected type: UInt</p>
             */
            FONT_SIZE,
            /**
             * \brief The font family to use for the component's text.
             *
             * <p>Expected type: String</p>
             */
            FONT_TYPE,
            /**
             * \brief The height of the component's renderable area.
             *
             * <p>Expected type: UInt</p>
             */
            HEIGHT,
            /**
             * \brief The layout direction for the component.
             *
             * <p>Expected type: UInt (enum)</p>
             *
             * \see PB:UI:Layout
             */
            LAYOUT,
            /**
             * \brief The location from which POS_X, POS_Y are set by.
             *
             * <p>Expected type: UInt (enum)</p>
             *
             * \see PB:UI:Origin
             */
            ORIGIN,
            /**
             * \brief The component's distance from the edge of the screen on the X axis.
             *
             * <p>Affected by it's Origin location<p>
             *
             * <p>Expected type: UInt</p>
             */
            POS_X,
            /**
             * \brief The component's distance from the edge of the screen on the Y axis.
             *
             * <p>Affected by it's Origin location<p>
             *
             * <p>Expected type: UInt</p>
             */
            POS_Y,
            /**
             * \brief The layer order of the UI component.
             *
             * <p>Values closer to 0 make the component more "on top".</p>
             *
             * <p>Expected type: UInt</p>
             */
            POS_Z,
            /**
             * \brief The content of the component's text.
             *
             * <p>Expected type: String</p>
             */
            TEXT_CONTENT,
            /**
             * \brief The width of the component's renderable area.
             *
             * <p>Expected type: UInt</p>
             */
            WIDTH,
        };

        /**
         * Allowed values for the Origin {\link PB::UI::Attribute}.
         */
        enum Origin
        {
            BOTTOM_LEFT,
            TOP_LEFT,
            TOP_RIGHT,
            BOTTOM_RIGHT,
        };

        /**
         * Allowed values for the Layout {\link PB::UI::Attribute}.
         */
        enum Layout
        {
            HORIZONTAL,
            VERTICAL,
        };
    }

    class PUPPET_BOX_API UIComponentAttributes
    {
    public:
        virtual void setUIntAttribute(UI::Attribute attributeName, std::uint32_t value) = 0;

        virtual Result<std::uint32_t> getUIntAttribute(UI::Attribute attributeName) const = 0;

        virtual void setIntAttribute(UI::Attribute attributeName, std::int32_t value) = 0;

        virtual Result<std::int32_t> getIntAttribute(UI::Attribute attributeName) const = 0;

        virtual void setStringAttribute(UI::Attribute attributeName, const std::string& value) = 0;

        virtual Result<std::string> getStringAttribute(UI::Attribute attributeName) const = 0;
    };

    /**
     * Basic interface for a UI component.
     *
     * <p>Adopts the {\link UIComponentAttributes} interface to allow further modification after
     * instantiation.</p>
     *
     * TODO: UIComponentAttributes -> UIComponent is a messy relationship
     */
    class PUPPET_BOX_API UIComponent : public UIComponentAttributes
    {
    public:
        UIComponent();

        static std::unique_ptr<UIComponentAttributes> createUIComponentAttributes();

        /**
         * \brief Invoked to initialize the state of a component and esnure functionality.
         *
         * @return True if the UI component successfully initialized and is ready to use, False
         * otherwise.
         */
        virtual bool init();

        /**
         * \brief The "Update" phase is where checks should be done to see if the component's
         * attributes have been changed, and update it's state.
         *
         * TODO: Implement listeners for the various attributes?
         *
         * \param deltaTime The amount of tie that has passed (in milliseconds) since the last update call.
         */
        virtual void update(float deltaTime) = 0;

        /**
         * \brief The "Render" phase is simply drawing to the screen, no state changes should be done,
         * and calculations should be at a bare minimum, preferably none.
         */
        virtual void render() const = 0;

        virtual bool addComponent(std::shared_ptr<UIComponent> component);

        void setAttributes(std::unique_ptr<UIComponentAttributes> attributes);

        void setUIntAttribute(UI::Attribute attributeName, std::uint32_t value) override;

        Result<std::uint32_t> getUIntAttribute(UI::Attribute attributeName) const override;

        void setIntAttribute(UI::Attribute attributeName, std::int32_t value) override;

        Result<std::int32_t> getIntAttribute(UI::Attribute attributeName) const override;

        void setStringAttribute(UI::Attribute attributeName, const std::string& value) override;

        Result<std::string> getStringAttribute(UI::Attribute attributeName) const override;

    private:
        std::unique_ptr<UIComponentAttributes> attributes_;
    };
}