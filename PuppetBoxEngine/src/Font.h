#pragma once

#include <unordered_map>
#include <utility>

#include "puppetbox/DataStructures.h"
#include "ImageReference.h"

namespace PB
{
    /**
     * Holds information related to a particular glyph, including a reference for rendering.
     */
    struct TypeCharacter
    {
        std::uint8_t character = 0;
        ImageReference image{0};
        uivec2 size{};
        uivec2 atlasPosition{};
        ivec2 bearing{};
        std::uint32_t advance = 0;
    };

    /**
     * Holds a collection of references for rendering glyphs of a particular font type.
     */
    class Font
    {
    public:
        /**
         * \brief Default initializer to use to create an "empty" font object, does NOT set
         * initialization to true.
         */
        Font() = default;

        /**
         * \brief Constructor to use to initialize a valid font object, sets initialization to true.
         *
         * \param characterMap The {\link unordered_map} of characters for this font set.
         * \param fontSize     The font size that was used when loading this font.
         */
        Font(std::unordered_map<std::int8_t, TypeCharacter> characterMap, std::uint32_t fontSize)
                : characterMap_(std::move(characterMap)), fontSize_(fontSize), initialized_(true) {};

        /**
         * \brief Gets a {\link TypeCharacter} object for the given character.
         *
         * \param c The character to get the associated {\link TypeCharacter} for.
         * \return The associated {\link TypeCharacter} for the given character.
         */
        Result<TypeCharacter> getCharacter(std::int8_t c) const
        {
            Result<TypeCharacter> result;

            if (characterMap_.find(c) != characterMap_.end())
            {
                result.hasResult = true;
                result.result = characterMap_.at(c);
            }

            return result;
        }

        /**
         * \brief The size is determined by the value used when the font is
         * loaded for the first time.
         *
         * \return The size used when the font was first loaded.
         */
        std::uint32_t fontSize() const
        {
            return fontSize_;
        }

        /**
         * /brief Indicates if the font object has been initialized correctly, or
         * if it was initialized with the default constructor.
         *
         * @return True if the font was initialized with both a character set and font size,
         * False otherwise.
         */
        bool isInitialized() const
        {
            return initialized_;
        }

    private:
        std::unordered_map<std::int8_t, TypeCharacter> characterMap_{};
        std::uint32_t fontSize_ = 0;
        bool initialized_ = false;
    };
}