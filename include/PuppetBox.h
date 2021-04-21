#pragma once

#include <string>

#ifdef PUPPETBOXENGINE_EXPORTS
#	define PUPPET_BOX_API __declspec(dllexport)
#else
#	define PUPPET_BOX_API __declspec(dllimport)
#endif

#include "puppetbox/AbstractSceneHandler.h"
#include "puppetbox/BasicMessage.h"

namespace PB
{
	class AbstractSceneHandler;

	extern PUPPET_BOX_API void Init(std::string windowTitle, uint32_t windowWidth, uint32_t windowHeight);
	extern PUPPET_BOX_API void CreateScene(std::string sceneName);
	extern PUPPET_BOX_API void SetActiveScene(std::string sceneName);
	extern PUPPET_BOX_API void SetSceneHandler(AbstractSceneHandler* sceneHandler);
	extern PUPPET_BOX_API void LoadAssetLibrary(std::string archiveName);
	extern PUPPET_BOX_API void Run();
}