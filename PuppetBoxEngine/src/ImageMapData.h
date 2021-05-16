#pragma once

#include <string>

struct ImageMapData
{
	std::string image;
	std::uint32_t width = 0;
	std::uint32_t height = 0;
	std::uint32_t xOffset = 0;
	std::uint32_t yOffset = 0;
};