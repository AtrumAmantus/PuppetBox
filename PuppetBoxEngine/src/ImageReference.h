#pragma once

#include <string>

#include <glad/glad.h>

namespace PB
{
    namespace
    {
        /**
         * \brief Maps generic texture ID references to OpenGL specific identifiers.
         *
         * \param locationId The generic texture location ID reference.
         * \return The OpenGL specific texture location ID reference.
         */
        std::int32_t mapToGLLocation(std::int32_t locationId)
        {
            std::int32_t glLocation = 0;

            switch(locationId)
            {
                case 0:
                    glLocation = GL_TEXTURE0;
                    break;
            }

            return glLocation;
        }
    }

    /**
    * \brief OpenGL API specific implementation for ImageReference data used to reference stored image data.
    */
    class ImageReference
    {
    public:
        std::uint32_t width = 0;
        std::uint32_t height = 0;
        bool requiresAlphaBlending = false;
    public:
        explicit ImageReference(std::uint32_t referenceId) : referenceId_(referenceId) {};

        /**
        * \brief Instructs the API to use this image data to populate the uniform variable at the given
        * uniform variable location for future rendering calls.
        *
        * \param locationId	The location of the uniform variable to populate with the image data for
        * future rendering calls.
        */
        void use(std::int32_t locationId) const
        {
            std::int32_t glLocation = mapToGLLocation(locationId);

            // Bind for use in current render
            if (requiresAlphaBlending)
            {
                glEnable(GL_BLEND);
            }
            glActiveTexture(glLocation);
            glBindTexture(GL_TEXTURE_2D, referenceId_);
        };

        /**
        * \brief Instructs the API to stop using this image data to populate the uniform variable at the given
        * uniform variable location for future rendering calls.
        *
        * \param locationId	The location of the uniform variable to stop populating with the image data for
        * future rendering calls.
        */
        // Keeping this non static for consistency with ImageReference#use()
        void unuse(std::int32_t locationId) const // NOLINT(readability-convert-member-functions-to-static)
        {
            std::int32_t glLocation = mapToGLLocation(locationId);

            // Unbind after use in current render
            glDisable(GL_BLEND);
            glActiveTexture(glLocation);

            // Unbind texture at glLocation
            glBindTexture(GL_TEXTURE_2D, 0);
        };

        /**
        * \brief Releases the memory used to previously store the image data referenced by this object instance.
        */
        void free()
        {

        };
    private:
        std::uint32_t referenceId_ = 0;
    };
}