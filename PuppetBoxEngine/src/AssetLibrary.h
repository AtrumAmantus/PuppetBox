#pragma once

#include <string>
#include <vector>

#include "puppetbox/IAnimationCatalogue.h"
#include "puppetbox/SceneObject.h"

#include "puppetbox/IModel.h"
#include "AssetArchive.h"
#include "Font.h"
#include "IGfxApi.h"
#include "ImageReference.h"
#include "Logger.h"
#include "Material.h"
#include "Mesh.h"
#include "RenderedMesh.h"
#include "Shader.h"

namespace PB
{
    namespace Asset
    {
        enum Type
        {
            UNKNOWN,
            MODEL_2D
        };
    }

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
        * \brief Attempts to load the archive with the given name so that it's contained assets can be used in future
        * asset load calls.
        *
        * \param archiveName	The name of the archive to attempt to load.
        *
        * \return True if the archive was loaded successfully, False otherwise.
        */
        bool loadArchive(const std::string& archiveName);

        /**
        * \brief Attempts to load the given asset into the provided SceneObject.  This logic handles loading the mesh,
        * textures, animations, etc. for the given SceneObject.
        *
        * The path is a virtual path to the asset, beginning with the actual AssetArchive name.
        *
        * \param assetPath		Virtual path to the asset, beginning with actual AssetArchive name.
        * \param sceneObject	Pointer to the SceneObject that will be impregnated with the requested asset references.
        * \param type			The type of asset to be loaded, which will dictate asset loading configurations.  Currently
        * only supports MODEL_2D.
        *
        * \return True if the asset was loaded successfully, False otherwise.
        */
        bool loadModelAsset(const std::string& assetPath, SceneObject* sceneObject, Asset::Type type);

        /**
        * \brief Loads a Shader asset given by the provided virtual asset path.
        *
        * \param assetPath	Virtual path to the requested asset.
        * \param error		Flag indicating an error occurred if set to True.
        *
        * \return The loaded Shader object for the respective virtual asset path, or an empty
        * object if an error occurred loading the asset.
        */
        Shader loadShaderAsset(const std::string& assetPath, bool* error);

        /**
         * \brief Loads a set of animation assets, sets are a collection of animations associated with
         * a particular skeleton.
         *
         * \param assetPath Path to the animation set.
         * \param map       The map to add the animations of the animation set to.
         * \return True if the animation set was loaded successfully, False otherwise.
         */
        bool loadAnimationSetAsset(const std::string& assetPath, std::unordered_map<std::string, IAnimation*>& map);

        /**
         * \brief Loads an individual animation asset.
         *
         * \param name      The desired reference name for the animation.
         * \param assetPath The path to the desired animation.
         * \param map       The map to add the animation to, using the given name.
         * \return True if the animation was loaded and added to the map successfully, False otherwise.
         */
        bool loadAnimationAsset(const std::string& animName, const std::string& assetPath,
                                std::unordered_map<std::string, IAnimation*>& map);

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

        /**
        * \brief Loads a Mesh asset given by the provided virtual asset path.
         *
        * \pa&ram assetPath	Virtual path to the requested asset.
        * \param error		Flag indicating an error occurred if set to True.
        *
        * \return The loaded Mesh object for the respective virtual asset path, or an empty
        * object if an error occurred loading the asset.
        */
        Mesh loadMeshAsset(const std::string& assetPath, bool* error);

        /**
        * \brief Loads an ImageReference asset given by the provided virtual asset path.
        *
        * \param assetPath		Virtual path to the requested asset.
        * \param imageOptions	Image loading options to use when loading into memory.
        * \param error			Flag indicating an error occurred if set to True.
        *
        * \return The loaded ImageReference object for the respective virtual asset path, or an empty
        * object if an error occurred loading the asset.
        */
        ImageReference loadImageAsset(const std::string& assetPath, ImageOptions imageOptions, bool* error);

    private:
        std::string archiveRoot_;
        std::shared_ptr<IGfxApi> gfxApi_;
        FontLoader* fontLoader_;
        std::unordered_map<std::string, AssetArchive> assetArchives_{};
        std::unordered_map<std::string, Mesh> loadedMeshes_{};
        std::unordered_map<std::string, Material> loadedMaterials_{};
        std::unordered_map<std::string, ImageReference> loadedImages_{};
        std::unordered_map<std::string, ModelData2D> loadedModelData2D_{};
        std::unordered_map<std::string, Shader> loadedShaders_{};
        std::unordered_map<std::string, Font> loadedFonts_{};
    private:

        /**
        * \brief Loads a Material asset given by the provided virtual asset path.
        *
        * \param assetPath	Virtual path to the requested asset.
        * \param error		Flag indicating an error occurred if set to True.
        *
        * \return The loaded Material object for the respective virtual asset path, or an empty
        * object if an error occurred loading the asset.
        */
        Material loadMaterialAsset(const std::string& assetPath, bool* error);

        /**
        * \brief Loads a ModelData2D asset given by the provided virtual asset path.
        *
        * \param assetPath	Virtual path to the requested asset.
        * \param error		Flag indicating an error occurred if set to True.
        *
        * \return The loaded ModelData2D object for the respective virtual asset path, or an empty
        * object if an error occurred loading the asset.
        */
        ModelData2D loadModelData2DAsset(const std::string& assetPath, bool* error);

        /**
        * \brief Loads the given scene object with 2D asset data.
        *
        * \param assetPath		Virtual path to the requested asset.
        * \param sceneObject	The instantiated scene object to load with asset data.
        *
        * \return True if the scene object was successfully loaded with assets, False otherwise.
        */
        bool load2DSceneObject(const std::string& assetPath, SceneObject* sceneObject);

        bool buildMeshAndBones(ModelData2D modelData, std::string parent, std::uint32_t depth,
                               std::unordered_map<std::string, BoneMap>& bones,
                               std::unordered_map<std::string, RenderedMesh*>& meshes);
    };
}