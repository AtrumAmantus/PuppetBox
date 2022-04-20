#pragma once

#include <cstdint>
#include <string>
#include <vector>

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
        namespace Attribute
        {
            enum String
            {
                /**
                 * \brief The font family to use for the component's text.
                 */
                FONT_TYPE,
                /**
                 * \brief The content of the component's text.
                 */
                TEXT_CONTENT,
            };

            enum UInteger
            {
                /**
                 * \brief The size to render the component's text at.
                 */
                FONT_SIZE,
                /**
                 * \brief The height of the component's renderable area.
                 */
                HEIGHT,
                /**
                 * \brief The layout direction for the component.
                 *
                 * \see PB:UI:Layout
                 */
                LAYOUT,
                /**
                 * \brief The location from which POS_X, POS_Y are set by.
                 *
                 * \see PB:UI:Origin
                 */
                ORIGIN,
                /**
                 * \brief The component's distance from the edge of the screen on the X axis.
                 *
                 * <p>Affected by it's Origin location<p>
                 */
                POS_X,
                /**
                 * \brief The component's distance from the edge of the screen on the Y axis.
                 *
                 * <p>Affected by it's Origin location<p>
                 */
                POS_Y,
                /**
                 * \brief The layer order of the UI component.
                 *
                 * <p>Values closer to 0 make the component more "on top".</p>
                 */
                POS_Z,
                /**
                 * \brief The width of the component's renderable area.
                 */
                WIDTH,
            };

            enum Float
            {
                /**
                 * \brief Spacing multiplier for spacing between letters.
                 *
                 * <p>1.0 means default spacing</p>
                 */
                LETTER_SPACE,
                /**
                 * \brief Spacing multiplier for space between words (width of space character).
                 *
                 * <p>1.0 means default spacing</p>
                 */
                WORD_SPACE,
            };

            enum Boolean
            {
                /**
                 * \brief Denotes if the text should "wrap" around to the next line or not.
                 */
                WORD_WRAP,
            };
        }
    }

    /**
     * Base class for a UI component.
     */
    class PUPPET_BOX_API UIComponent
    {
    public:
        UIComponent(UUID id);

        virtual ~UIComponent();

        /**
         * \brief Invoked to initialize the state of a component and esnure functionality.
         *
         * @return True if the UI component successfully initialized and is ready to use, False
         * otherwise.
         */
        bool init();

        /**
         * \brief The "Update" phase is where checks should be done to see if the component's
         * attributes have been changed, and update it's state.
         *
         * \param deltaTime The amount of tie that has passed (in seconds) since the last update call.
         */
        virtual void update(float deltaTime) = 0;

        /**
         * \brief The "Render" phase is simply drawing to the screen, no state changes should be done,
         * and calculations should be at a bare minimum, preferably none.
         */
        virtual void render() const = 0;

        virtual bool addComponent(PB::UUID uuid);

    protected:
        virtual bool inits();

        virtual void updateStringAttribute(UI::Attribute::String attribute, std::string value);

        virtual void updateUIntegerAttribute(UI::Attribute::UInteger attribute, std::uint32_t value);

        virtual void updateFloatAttribute(UI::Attribute::Float attribute, float value);

        virtual void updateBooleanAttribute(UI::Attribute::Boolean attribute, bool value);

    private:
        PB::UUID id_;
        std::vector<UUID> subscriptions_{};
    };
}