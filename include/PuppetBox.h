#pragma once

#include <string>

#include "puppetbox/AbstractSceneHandler.h"
#include "puppetbox/Constants.h"
#include "puppetbox/SceneObject.h"
#include "puppetbox/TypeDef.h"
#include "puppetbox/UIComponent.h"

namespace PB
{
    /**
     * \brief Initialize app context, create window, etc.
     *
     * \param windowTitle The label used in the window's title
     * \param windowWidth The initial width of the window
     * \param windowHeight The initial height of the window
     */
    extern PUPPET_BOX_API void Init(const std::string& windowTitle, std::int32_t windowWidth, std::int32_t windowHeight, std::int32_t renderDepth);

    /**
     * \brief Create a new scene referenced by the given string
     *
     * \param sceneName The name to reference the scene by
     */
    extern PUPPET_BOX_API void CreateScene(const std::string& sceneName);

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
     * \return True if the asset pack was loaded successfully, False otherwise.
     */
    extern PUPPET_BOX_API bool LoadAssetPack(const std::string& archiveName);

    /**
     * \brief Loads a font asset from the given fully qualified asset path.
     *
     * <p>The given font-size is only considered the first time the font is loaded.
     *
     * \param fontPath The fully qualified asset path of the font asset.
     * \param fontSize The desired fontSize to load the font at.
     * \return True if the font asset was loaded successfully, False otherwise.
     */
    extern PUPPET_BOX_API bool LoadFontAsset(const std::string& fontPath, std::uint8_t fontSize);

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

    /**
     * \brief Loads the animations associated with the given asset path.
     *
     * \param assetPath The path to the animation assets to load.
     * \return True if the animation asset was loaded successfully, False otherwise.
     */
    extern PUPPET_BOX_API bool LoadAnimationsPack(const std::string& assetPath);

    /**
     * \brief Initiates start of core engine, input processors, and render loops.
     */
    extern PUPPET_BOX_API void Run();

    /**
    * \brief Helper function for getting the ascii character the given key code represents.
    *
    * \param code	    The arbitrary key code to be maped to an ascii character.
    * \param shifted    Indicates if the shift modified version of the chracter is desired.
    * Defaulted to true.
    *
    * \return The int value for the ascii character the key code represents.
    */
    extern PUPPET_BOX_API std::int8_t GetCharFromCode(std::uint8_t code, bool shifted = false);

    /**
     * \brief Creates a {\link UIComponent} of the given {\link UI::Type}.
     *
     * \param uiComponentType The type of {\link UIComponent} to create.
     * \param attributes      The {\link UIComponentAttributes} to associate with the created
     * {\link UIComponent}.
     * \param error           Error flag to indicate if the component was created successfully.
     * \return The created {\link UIComponent}.
     */
    extern PUPPET_BOX_API UIComponent*
    CreateUIComponent(UI::Type uiComponentType, std::unique_ptr<UIComponentAttributes> attributes, bool* error);
}