#pragma once

#include <cstdint>

namespace PB
{
	/**
	* \brief Helper object used to facilitate transfer of loaded image file data between application logic.
	*/
	struct ImageData
	{
		std::uint8_t* bufferData;
		// Buffer size is derived from width & height of image
		std::int32_t width;
        std::int32_t height;
        std::int32_t numChannels;
		/**
		* \brief Used to free the buffer data of the image data once it is no longer needed.
		*/
		void drop()
		{
			free(bufferData);
			bufferData = nullptr;
		}
	};
}