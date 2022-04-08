#pragma once

#include <string>
#include <vector>

#include "puppetbox/IAnimationCatalogue.h"
#include "puppetbox/ImageMapReference.h"

#include "AssetArchive.h"
#include "Font.h"
#include "IGfxApi.h"
#include "ImageReference.h"
#include "Logger.h"
#include "Material.h"
#include "Mesh.h"
#include "Shader.h"

namespace PB
{
    class IAnimation;

    /**
    * \brief Manages loading and organization of all asset archives.  Asset loading is all done through
    * the AssetLibrary, which will then facilitate specific archive loads and caching of assets.
    *
    * Currently only SceneObject assets can be loaded from the AssetLibrary, future assets will need to be added where necessary.
    */
    class AssetLibrary
    {
    public:
        /**
        * \brief Create a AssetLibrary instance, providing the root directory for where all archives should be fetched, and
        * IGfxApi that will be used for loading of assets.
        *
        * \param archiveRoot	The root directory that AssetLibrary should look for archives when they are loaded.
        * \param gfxApi			The derived gfxApi that will be used for loading assets.
        */
        AssetLibrary(std::string archiveRoot, std::shared_ptr<IGfxApi> gfxApi, FontLoader* fontLoader);

        /**
         * \brief Initializes the asset library, such as loading default assets.
         *
         * \return True if the initialization was successful, False otherwise.
         */
        bool init();

        /**
        * \brief Attempts to load the archive with the given name so that it's contained assets can be used in future
        * asset load calls.
        *
        * \param archiveName	The name of the archive to attempt to load.
        *
        * \return True if the archive was loaded successfully, False otherwise.
        */
        bool loadArchive(const std::string& archiveName);

        BoneMap loadSkeletonAsset(const std::string& assetPath, bool* error);

        /**
        * \brief Loads a Mesh asset given by the provided virtual asset path.
         *
        * \param assetPath	Virtual path to the requested asset.
        * \param error		Flag indicating an error occurred if set to True.
        *
        * \return A reference to the render data for the loaded mesh of the given
        * virtual asset path, or a 0 if an error occurred loading the asset.
        */
        std::uint32_t loadMeshAsset(const std::string& assetPath, bool* error);

        std::uint32_t loadImageMapAsset(const std::string& assetPath, bool* error);

        std::uint32_t loadShaderAsset(const std::string& assetPath, bool* error);

        /**
         * \brief Loads a set of animation assets, sets are a collection of animations associated with
         * a particular skeleton.
         *
         * \param assetPath     Path to the animation set.
         * \param animationMap  The animationMap to add the animations of the animation set to.
         * \return True if the animation set was loaded successfully, False otherwise.
         */
        bool loadAnimationSetAsset(
                const std::string& assetPath,
                std::unordered_map<std::string, IAnimation*>& animationMap
        );

        /**
         * \brief Loads an individual animation asset.
         *
         * \param name          The desired reference name for the animation.
         * \param assetPath     The path to the desired animation.
         * \param animationMap  The animationMap to add the animation to, using the given name.
         * \return True if the animation was loaded and added to the animationMap successfully, False otherwise.
         */
        bool loadAnimationAsset(
                const std::string& animName,
                const std::string& assetPath,
                std::unordered_map<std::string, IAnimation*>& animationMap
        );

        /**
         * \brief Load the given font asset.
         *
         * <p>On the first load of a font, the given size dictates how large to create it's associated
         * glyph images.  Future calls used these cached images and the requested size is ignored.</p>
         *
         * \param assetPath  The path of the font to load.
         * \param fontSize   The size to load the font at.
         * \param fontLoader The {\link FontLoader} to use to load the font with.
         * \param error      Error flag to indicate if there was an issue loading the font.
         * \return The {\link Font} object associated with the loaded font.  Returns an uninitialized {\link Font}
         * object if the font failed to load.
         */
        Font loadFontAsset(const std::string& fontPath, std::uint8_t fontSize, bool* error);

    private:
        std::string archiveRoot_;
        std::shared_ptr<IGfxApi> gfxApi_;
        FontLoader* fontLoader_;
        std::unordered_map<std::string, AssetArchive> assetArchives_{};
        std::unordered_map<std::string, BoneMap> loadedSkeletons_{};
        std::unordered_map<std::string, std::uint32_t> loadedMeshes_{};
        std::unordered_map<std::string, std::uint32_t> loadedImageMaps_{};
        std::unordered_map<std::string, std::uint32_t> loadedShaders_{};
        std::unordered_map<std::string, Font> loadedFonts_{};
    };
}