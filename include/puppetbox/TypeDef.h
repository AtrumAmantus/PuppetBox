#pragma once

#ifdef PUPPETBOXENGINE_EXPORTS
#	define PUPPET_BOX_API __declspec(dllexport)
#else
#	define PUPPET_BOX_API __declspec(dllimport)
#endif