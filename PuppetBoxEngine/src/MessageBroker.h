#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

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
                for (auto& f: subscribers_.at(topicId))
                {
                    f(event);
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
         * \return The topic id for the subscribed event name.
         */
        std::uint32_t subscribe(std::string topicName, std::function<void(std::shared_ptr<void>)> consumer)
        {
            std::uint32_t topicId = registerTopic(topicName);

            LOGGER_DEBUG("Now listening for '" + topicName + "' events");

            if (subscribers_.find(topicId) == subscribers_.end())
            {
                subscribers_.insert(
                        std::pair<std::uint32_t, std::vector<std::function<void(std::shared_ptr<void>)>>>(
                                topicId,
                                std::vector<std::function<void(std::shared_ptr<void>)>>{}
                        )
                );
            }

            subscribers_.at(topicId).push_back(consumer);

            return topicId;
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
        std::unordered_map<std::string, std::uint32_t> topicIds_{};
        std::unordered_map<std::uint32_t, std::vector<std::function<void(std::shared_ptr<void>)>>> subscribers_{};
        std::uint32_t lastTopicId_ = 0;

    private:
        MessageBroker() = default;
    };
}