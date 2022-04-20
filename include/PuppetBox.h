#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <string>

#include "puppetbox/AbstractSceneGraph.h"
#include "puppetbox/Constants.h"
#include "puppetbox/Event.h"
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
    extern PUPPET_BOX_API void Init(
            const std::string& windowTitle,
            std::int32_t windowWidth,
            std::int32_t windowHeight,
            std::int32_t renderDepth);

    /**
     * \brief Injects {\link AbstractSceneGraph} data into an existing implementation class object and adding it to the
     * list of available scenes.
     *
     * \param scene The scene object to be populated.
     */
    extern PUPPET_BOX_API void CreateScene(std::shared_ptr<AbstractSceneGraph> scene);

    /**
     * \brief Set the currently active scene
     *
     * \param sceneName The name referencing the specified scene
     */
    extern PUPPET_BOX_API void SetActiveScene(const std::string& sceneName);

    /**
     * \brief Loads a special PuppetBox format asset package to be utilized in the scene
     *
     * \param archiveName The name of the asset pack to load
     * \return True if the asset pack was loaded successfully, False otherwise.
     */
    extern PUPPET_BOX_API bool LoadAssetPack(const std::string& archiveName);

    extern PUPPET_BOX_API std::uint32_t GetMeshAsset(const std::string& assetPath, bool* error);

    extern PUPPET_BOX_API std::uint32_t GetImageMapAsset(const std::string& assetPath, bool* error);

    extern PUPPET_BOX_API std::uint32_t GetShaderAsset(const std::string& assetPath, bool* error);

    extern PUPPET_BOX_API BoneMap GetSkeletonAsset(const std::string& assetPath, bool* error);

    /**
     * \brief Loads the animation associated with the given asset path.
     *
     * \param assetPath The path to the animation asset to load.
     * \return True if the animation asset was loaded successfully, False otherwise.
     */
    extern PUPPET_BOX_API bool LoadAnimationAsset(const std::string& assetPath);

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
     * \brief Preloads the animation keyframes into memory.
     *
     * \param animationPath The path to the desired animation to preload.
     * \param boneMap       The skeletal data to use when preloading the keyframes.
     *
     * \return True if the preload was successful, False otherwise.
     */
    extern PUPPET_BOX_API bool PreloadAnimationFrames(const std::string& animationPath, BoneMap& boneMap);

    /**
     * \brief Initiates start of core engine, input processors, and render loops.
     */
    extern PUPPET_BOX_API void Run(std::function<bool()> onReady);

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
     * \brief Publish an event on the internal messaging system using the topic name.
     *
     * \param topicName The event topic ID to publish to.
     * \param data      The data related to the event to send.
     * \return The topic id associated to the topic of the event that was submitted.
     */
    extern PUPPET_BOX_API std::uint32_t PublishEvent(const std::string& topicName, std::shared_ptr<void> data);

    /**
     * \brief Publish an event on the internal messaging system using the topic id.
     *
     * \param topicId   The event topic name to publish to.
     * @param data      The data related to the event to send.
     */
    extern PUPPET_BOX_API void PublishEvent(std::uint32_t topicId, std::shared_ptr<void> data);

    /**
     * \brief Subscribe to events for a given topic name to start processing them with
     * the given function reference.
     *
     * \param topicName The event topic name to subscribe to and start processing for.
     * \param callback  The function to process the data of events for the subscribed topic.
     * \return The topic id associated to the subscribed topic name.
     */
    extern PUPPET_BOX_API UUID SubscribeEvent(
            const std::string& topicName,
            std::function<void(std::shared_ptr<void>)> callback);

    /**
     * \brief Destroy the specific subscription associated with the given {\link PB::UUID}.
     *
     * \param uuid  The {\link PB::UUID} associated with the subscription to destroy.
     */
    extern PUPPET_BOX_API void Unsubscribe(UUID uuid);

    /**
     * \brief Registers the given topic name if it is not already, returning the ID for it.
     *
     * \param topicName The topic name to register
     * \return The newly registered topic ID, or the existing topic ID if the topic name was already registered.
     */
    extern PUPPET_BOX_API std::uint32_t RegisterTopic(const std::string& topicName);

    /**
     * \brief Registers an event listener to be used with the network thread for sending events over the
     * network.  The registered topic ID will be listened for and given transformer used to convert the event
     * into a data stream that can be sent over the network.
     *
     * \param topicName The topic name for events to listen for.
     * \param writer    The transformer to use to convert the event into network data.
     */
    extern PUPPET_BOX_API void RegisterNetworkEventWriter(const std::string& topicName, pb_NetworkEventWriter writer);

    /**
     * \brief Registers a network reader function to be used for reading data off the network.  Only one
     * network reader can be registered (active) at a time.
     *
     * \param reader The network reader to register and utilize to read network data.
     */
    extern PUPPET_BOX_API void RegisterNetworkEventReader(pb_NetworkEventReader reader);

    /**
     * \brief Removes the current network reader function used to read network data.  All network
     * data will be ignored after the reader has been cleared.
     */
    extern PUPPET_BOX_API void ClearNetworkEventReader();
}