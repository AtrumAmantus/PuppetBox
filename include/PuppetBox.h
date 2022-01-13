#pragma once

#include <string>

#include "puppetbox/AbstractSceneHandler.h"
#include "puppetbox/Constants.h"
#include "puppetbox/IAnimationCatalogue.h"
#include "puppetbox/SceneObject.h"
#include "puppetbox/TypeDef.h"

namespace PB
{
    /**
     * \brief Initialize app context, create window, etc.
     *
     * \param windowTitle The label used in the window's title
     * \param windowWidth The initial width of the window
     * \param windowHeight The initial height of the window
     */
    extern PUPPET_BOX_API void Init(const std::string& windowTitle, int32_t windowWidth, int32_t windowHeight);

    /**
     * \brief Create a new scene referenced by the given string
     *
     * \param sceneName The name to reference the scene by
     */
    extern PUPPET_BOX_API void CreateScene(const std::string& sceneName);

    /**
    * \brief Set the camera mode for the scene associated to the given reference.
    *
    * \param sceneName	The reference name of the scene to affect.
    * \param mode		The camera mode to apply to the referenced scene.
    */
    extern PUPPET_BOX_API void SetSceneCameraMode(const std::string& sceneName, SceneView::Mode mode);

    /**
     * \brief Set the currently active scene
     *
     * \param sceneName The name referencing the specified scene
     */
    extern PUPPET_BOX_API void SetActiveScene(const std::string& sceneName);

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
    extern PUPPET_BOX_API void LoadAssetPack(const std::string& archiveName);

    /**
     * \brief Injects the base assets into the given scene object.
     *
     * \param assetPath The path to the asset to inject
     * \param sceneObject The SceneObject to inject assets into, must not be a nullptr.
     * \param type Specifies what type the requested asset is.
     *
     * \return True if the SceneObject was successfully injected with assets, False otherwise.
     */
    extern PUPPET_BOX_API bool
    CreateSceneObject(const std::string& assetPath, SceneObject* sceneObject, LibraryAsset::Type type);

    extern PUPPET_BOX_API IAnimationCatalogue* CreateAnimationCatalogue();

    /**
     * \brief Initiates start of core engine, input processors, and render loops.
     */
    extern PUPPET_BOX_API void Run();

    /**
    * \brief Helper function for getting the ascii character the given key code represents.
    *
    * \param code	The arbitrary key code to be maped to an ascii character.
    *
    * \return The int value for the ascii character the key code represents.
    */
    extern PUPPET_BOX_API std::int8_t GetCharFromCode(std::uint8_t code);
}