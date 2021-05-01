#pragma once

#include <string>

#include "ImageReference.h"
#include "Shader.h"

namespace PB
{
	struct Material
	{
		std::string diffuseId;
		std::string emissionId;
		std::string specularId;
		std::string normalId;
		std::string shaderId = "unspecified";
		uint32_t specularValue = 0;
		float emissionValue = 0;
		ImageReference diffuseMap{ 0 };
		Shader shader{""};
	};
}