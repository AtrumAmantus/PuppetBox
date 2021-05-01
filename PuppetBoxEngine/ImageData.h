#pragma once

#include <stdint.h>

namespace PB
{
	struct ImageData
	{
		uint8_t* bufferData;
		// Buffer size is derived from width & height of image
		int32_t width;
		int32_t height;
		int32_t numChannels;
		void drop()
		{
			free(bufferData);
			bufferData = nullptr;
		}
	};
}