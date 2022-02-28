#pragma once

#include <atomic>
#include <functional>
#include <string>
#include <concurrent_unordered_map.h>
#include <vector>

#include "Utilities.h"

/**
 * Handles messaging communication within the base engine and implementing application.
 */
namespace PB
{
    class MessageBroker
    {
    public:
        /**
         * \brief Returns the singleton instance of the {\link MessageBroker}, creating it first if
         * it didn't already exist.
         *
         * \return The singleton instance of the {\link MessageBroker}
         */
        static MessageBroker& instance()
        {
            static MessageBroker instance;

            return instance;
        }

        /**
         * \brief Publishes an event by topic name to all current subscribers, executing
         * their registered callbacks.
         *
         * \param topic The topic name to publish the event to.
         * \param event The event to send to all subscriber's callbacks.
         * \return The topic id for the published event name.
         */
        std::uint32_t publish(std::string topicName, std::shared_ptr<void> event)
        {
            std::uint32_t topicId = registerTopic(topicName);

            publish(topicId, event);

            return topicId;
        }

        /**
         * \brief Publishes an event by topic ID to all current subscribers, executing
         * their registered callbacks.
         *
         * \param topicId The topic ID to publish the event to.
         * \param event   The event to send to all subscriber's callbacks.
         */
        void publish(std::uint32_t topicId, std::shared_ptr<void> event)
        {
            if (subscribers_.find(topicId) != subscribers_.end())
            {
                auto& topicSubscribers = subscribers_.at(topicId);

                for (auto& f: topicSubscribers)
                {
                    f.second(event);
                }
            }
            else
            {
                if (topicId == 0)
                {
                    LOGGER_WARN("Event published to unregistered topic");
                }
                else
                {
                    std::string topicName = "";

                    for (auto entry: topicIds_)
                    {
                        if (entry.second == topicId)
                        {
                            topicName = entry.first;
                        }
                    }

                    LOGGER_WARN("Event published to registered topic, '" + topicName + "', but no one is listening!");
                }
            }
        };

        /**
         * \brief Subscribes a callback function to the given topic name to be used in future
         * published event handling.
         *
         * \param topicName The topic name to subscribe the callback to.
         * \param consumer  The callback to be invoked for published events of this topic.
         * \return A UUID representing the subscription that can be used to remove it again later.
         */
        UUID subscribe(std::string topicName, std::function<void(std::shared_ptr<void>)> consumer)
        {
            std::uint32_t topicId = registerTopic(topicName);

            LOGGER_DEBUG("Now listening for '" + topicName + "' events");

            if (subscribers_.find(topicId) == subscribers_.end())
            {
                subscribers_.insert(
                        std::pair<std::uint32_t, std::unordered_map<UUID, std::function<void(std::shared_ptr<void>)>>>(
                                topicId,
                                std::unordered_map<UUID, std::function<void(std::shared_ptr<void>)>>{}
                        )
                );
            }

            UUID uuid = RandomUtils::uuid();

            subscribers_.at(topicId).insert(
                    std::pair<UUID, std::function<void(std::shared_ptr<void>)>>{uuid, consumer}
            );

            subscriptionToTopicId_.insert(
                    std::pair<UUID, std::uint32_t>{uuid, topicId}
            );

            return uuid;
        };

        /**
         * \brief Destroys any existing subscription associated with the given {\link PB::UUID}.
         *
         * \param uuid  The {\link PB::UUID} associated with the subscription to destroy.
         */
        void unsubscribe(UUID uuid)
        {
            auto iter = subscriptionToTopicId_.find(uuid);

            if (iter != subscriptionToTopicId_.end())
            {
                std::uint32_t topicId = iter->second;

                std::string topicName;

                for (auto t : topicIds_)
                {
                    if (t.second == topicId)
                    {
                        topicName = t.first;
                    }
                }

                subscribers_.at(topicId).erase(uuid);
                subscriptionToTopicId_.unsafe_erase(uuid);

                LOGGER_DEBUG("Stopped listening for '" + topicName + "' events");
            }
        };

        /**
         * \brief Registers a topic with the message broker, associating it with a
         * topic ID if it is not already associated with one.
         *
         * \param topicName The name of the topic to register.
         * \return The new or existing topic ID that is associated with the topic name.
         */
        std::uint32_t registerTopic(std::string topicName)
        {
            if (topicIds_.find(topicName) == topicIds_.end())
            {
                topicIds_.insert(
                        std::pair<std::string, std::uint32_t>{topicName, ++lastTopicId_}
                );

                LOGGER_DEBUG("Event '" + topicName + "' now registered with ID (" + std::to_string(lastTopicId_) + ")");
            }

            return topicIds_.at(topicName);
        };

    public:
        MessageBroker(MessageBroker const&) = delete;

        void operator=(MessageBroker const&) = delete;

    private:
        Concurrency::concurrent_unordered_map<std::string, std::uint32_t> topicIds_{};
        Concurrency::concurrent_unordered_map
                <std::uint32_t, std::unordered_map<UUID, std::function<void(std::shared_ptr<void>)>>> subscribers_{};
        Concurrency::concurrent_unordered_map<UUID, std::uint32_t> subscriptionToTopicId_{};
        std::atomic<uint32_t> lastTopicId_ = 0;

    private:
        MessageBroker() = default;
    };
}