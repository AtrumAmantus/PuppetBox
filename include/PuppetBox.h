#pragma once

#include <string>

#ifdef PUPPETBOXENGINE_EXPORTS
#	define PUPPET_BOX_API __declspec(dllexport)
#else
#	define PUPPET_BOX_API __declspec(dllimport)
#endif

#include "puppetbox/AbstractSceneHandler.h"
#include "puppetbox/Constants.h"
#include "puppetbox/SceneObject.h"

namespace PB
{
	class SceneObject;

	/**
	 * \brief Initialize app context, create window, etc.
	 *
	 * \param windowTitle The label used in the window's title
	 * \param windowWidth The initial width of the window
	 * \param windowHeight The initial height of the window
	 */
	extern PUPPET_BOX_API void Init(std::string windowTitle, uint32_t windowWidth, uint32_t windowHeight);

	/**
	 * \brief Create a new scene referenced by the given string
	 *
	 * \param sceneName The name to reference the scene by
	 */
	extern PUPPET_BOX_API void CreateScene(std::string sceneName);

	/**
	* \brief Set the camera mode for the scene associated to the given reference.
	* 
	* \param sceneName	The reference name of the scene to affect.
	* \param mode		The camera mode to apply to the referenced scene.
	*/
	extern PUPPET_BOX_API void SetSceneCameraMode(std::string sceneName, SceneView::Mode mode);

	/**
	 * \brief Set the currently active scene
	 *
	 * \param sceneName The name referencing the specified scene
	 */
	extern PUPPET_BOX_API void SetActiveScene(std::string sceneName);

	/**
	 * \brief Set a derived scene handler for the active scene
	 *
	 * \param sceneHandler The derived SceneHandler to facilitate scene events
	 */
	extern PUPPET_BOX_API void SetSceneHandler(AbstractSceneHandler* sceneHandler);

	/**
	 * \brief Loads a special PuppetBox format asset package to be utilized in the scene
	 *
	 * \param archiveName The name of the asset pack to load
	 */
	extern PUPPET_BOX_API void LoadAssetPack(std::string archiveName);

	/**
	 * \brief Injects the base assets into the given scene object.
	 *
	 * \param assetPath The path to the asset to inject 
	 * \param sceneObject The SceneObject to inject assets into, must not be a nullptr.
	 * \param type Specifies what type the requested asset is.
	 */
	extern PUPPET_BOX_API void CreateSceneObject(std::string assetPath, SceneObject* sceneObject, LibraryAsset::Type type);

	/**
	 * \brief Initiates start of core engine, input processors, and render loops.
	 */
	extern PUPPET_BOX_API void Run();
}