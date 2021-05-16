#pragma once

#include <string>

#include "ImageMapData.h"
#include "ImageReference.h"
#include "Shader.h"

namespace PB
{
	/**
	* \brief Struct used to define Material data, including associated Shader and ImageReferences for GFX API
	* specific implementation.
	*/
	struct Material
	{
		ImageMapData diffuseData{};
		std::string emissionId;
		std::string specularId;
		std::string normalId;
		std::string shaderId = "unspecified";
        std::uint32_t specularValue = 0;
		float emissionValue = 0;
		bool requiresAlphaBlending = false;
		ImageReference diffuseMap{ 0 };
		Shader shader{""};
	};
}