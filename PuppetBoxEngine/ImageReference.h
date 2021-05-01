#pragma once

#include <string>

#include <glad/glad.h>

namespace PB
{
	class ImageReference
	{
	public:
		ImageReference(uint32_t id) : referenceId_(id) {};
		void use(int32_t textureId) const
		{
			// Bind for use in current render
			glActiveTexture(textureId);
			glBindTexture(GL_TEXTURE_2D, referenceId_);
		};
		void unuse(int32_t textureId) const
		{
			// Unbind after use in current render
			glActiveTexture(textureId);
			glBindTexture(GL_TEXTURE_2D, 0);
		};
		void free()
		{

		};
	private:
		uint32_t referenceId_;
	};
}