#pragma once

#include <atomic>
#include <functional>
#include <mutex>
#include <string>
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
        std::uint32_t publish(const std::string& topicName, std::shared_ptr<void> event)
        {
            std::unique_lock<std::mutex> mlock{mutex_};

            std::uint32_t topicId;

            auto topicItr = topicIds_.find(topicName);

            // Duplicating registration logic to maintain thread safety
            if (topicItr == topicIds_.end())
            {
                topicIds_.insert(std::pair<std::string, std::uint32_t>{topicName, ++lastTopicId_});

                topicId = lastTopicId_;

                LOGGER_DEBUG(
                        "Event '" + topicName + "' now registered with ID (" + std::to_string(lastTopicId_) + ")");
            }
            else
            {
                topicId = topicItr->second;
            }

            // Duplicating publish logic to maintain thread safety
            auto subscriberItr = subscribers_.find(topicId);

            if (subscriberItr != subscribers_.end())
            {
                auto& topicSubscribers = subscriberItr->second;

                for (auto& f: topicSubscribers)
                {
                    mlock.unlock();
                    f.second(event);
                    mlock.lock();
                }
            }

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
            std::unique_lock<std::mutex> mlock{mutex_};

            auto itr = subscribers_.find(topicId);

            if (itr != subscribers_.end())
            {
                auto& topicSubscribers = itr->second;

                for (auto& f: topicSubscribers)
                {
                    mlock.unlock();
                    f.second(event);
                    mlock.lock();
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

                    LOGGER_WARN(
                            "Event published to registered topic, '" + topicName + "', but no one is listening!");
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
        UUID subscribe(const std::string& topicName, std::function<void(std::shared_ptr<void>)> consumer)
        {
            std::unique_lock<std::mutex> mlock{mutex_};

            // Duplicate registration logic to maintain thread safety
            auto itr = topicIds_.find(topicName);

            if (itr == topicIds_.end())
            {
                itr = topicIds_.insert(
                        std::pair<std::string, std::uint32_t>{topicName, ++lastTopicId_}
                ).first;

                LOGGER_DEBUG(
                        "Event '" + topicName + "' now registered with ID (" + std::to_string(lastTopicId_) + ")");
            }

            LOGGER_DEBUG("Now listening for '" + topicName + "' events");

            if (subscribers_.find(itr->second) == subscribers_.end())
            {
                subscribers_.insert(
                        std::pair<std::uint32_t, std::unordered_map<UUID, std::function<void(
                                std::shared_ptr<void>)>>>(
                                itr->second,
                                std::unordered_map<UUID, std::function<void(std::shared_ptr<void>)>>{}
                        )
                );
            }

            UUID uuid = RandomUtils::uuid();

            subscribers_.at(itr->second).insert(
                    std::pair<UUID, std::function<void(std::shared_ptr<void>)>>{uuid, consumer}
            );

            subscriptionToTopicId_.insert(
                    std::pair<UUID, std::uint32_t>{uuid, itr->second}
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
            std::unique_lock<std::mutex> mlock{mutex_};
            auto iter = subscriptionToTopicId_.find(uuid);

            if (iter != subscriptionToTopicId_.end())
            {
                std::uint32_t topicId = iter->second;

                subscribers_.at(topicId).erase(uuid);
                subscriptionToTopicId_.erase(uuid);

#ifdef _DEBUG
                std::string topicName;

                for (auto t : topicIds_)
                {
                    if (t.second == topicId)
                    {
                        topicName = t.first;
                    }
                }

                LOGGER_DEBUG("Stopped listening for '" + topicName + "' events");
#endif
            }
        };

        /**
         * \brief Registers a topic with the message broker, associating it with a
         * topic ID if it is not already associated with one.
         *
         * \param topicName The name of the topic to register.
         * \return The new or existing topic ID that is associated with the topic name.
         */
        std::uint32_t registerTopic(const std::string& topicName)
        {
            std::unique_lock<std::mutex> mlock{mutex_};

            auto itr = topicIds_.find(topicName);

            if (itr == topicIds_.end())
            {
                itr = topicIds_.insert(
                        std::pair<std::string, std::uint32_t>{topicName, ++lastTopicId_}
                ).first;

                LOGGER_DEBUG(
                        "Event '" + topicName + "' now registered with ID (" + std::to_string(lastTopicId_) + ")");
            }

            return itr->second;
        };

    public:
        MessageBroker(MessageBroker const&) = delete;

        void operator=(MessageBroker const&) = delete;

    private:
        std::unordered_map<std::string, std::uint32_t> topicIds_{};
        std::unordered_map
                <std::uint32_t, std::unordered_map<UUID, std::function<void(std::shared_ptr<void>)>>> subscribers_{};
        std::unordered_map<UUID, std::uint32_t> subscriptionToTopicId_{};
        std::uint32_t lastTopicId_ = 0;
        std::mutex mutex_;

    private:
        MessageBroker() = default;
    };
}