#pragma once

#include <stdint.h>

namespace PB
{
	/**
	* \brief Helper object used to facilitate transfer of image data between application logic.
	*/
	struct ImageData
	{
		uint8_t* bufferData;
		// Buffer size is derived from width & height of image
		int32_t width;
		int32_t height;
		int32_t numChannels;
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