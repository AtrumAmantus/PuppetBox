#pragma once

#include <string>

#include <glad/glad.h>

namespace PB
{
	/**
	* \brief OpenGL API specific implementation for ImageReference data used to reference stored image data.
	*/
	class ImageReference
	{
	public:
		bool requiresAlphaBlending = false;
	public:
		/**
		* \brief Create an OpenGL specific ImageReference instance for referencing a stored image data.
		* 
		* \param id	The OpenGL id for referencing the stored image data.
		*/
		explicit ImageReference(std::uint32_t id) : referenceId_(id) {};

		/**
		* \brief Instructs the API to use this image data to populate the uniform variable at the given
		* uniform variable location for future rendering calls.
		* 
		* \param locationId	The location of the uniform variable to populate with the image data for
		* future rendering calls.
		*/
		void use(std::int32_t locationId) const
		{
			// Bind for use in current render
			glActiveTexture(locationId);
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
			// Unbind after use in current render
			glActiveTexture(locationId);
			glBindTexture(GL_TEXTURE_2D, 0);
		};

		/**
		* \brief Releases the memory used to previously store the image data referenced by this object instance.
		*/
		void free()
		{

		};
	private:
        std::uint32_t referenceId_;
	};
}