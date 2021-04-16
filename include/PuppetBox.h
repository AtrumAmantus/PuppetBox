#pragma once

#include <string>

#ifdef PUPPETBOXENGINE_EXPORTS
#	define PUPPET_BOX_API __declspec(dllexport)
#else
#	define PUPPET_BOX_API __declspec(dllimport)
#endif

namespace PB
{
	extern "C" PUPPET_BOX_API void Init(std::string windowTitle, uint32_t windowWidth, uint32_t windowHeight);
}