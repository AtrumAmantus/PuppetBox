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
    class MessageBroker;

    namespace
    {
        MessageBroker* instance = nullptr;
    }

    class MessageBroker
    {
    public:
        /**
         * \brief Creates a singleton instance of the messaging application, must be called before
         * any other methods.
         */
        static void init()
        {
            instance = new MessageBroker{};
        };

        /**
         * \brief Publishes an event by topic name to all current subscribers, executing
         * their registered callbacks.
         *
         * \param topic The topic name to publish the event to.
         * \param data  The data to send to all subscriber's callbacks.
         * \return The topic id for the published event name.
         */
        static std::uint32_t publish(std::string topic, std::shared_ptr<void> data)
        {
            return instance->publishI(topic, data);
        }

        /**
         * \brief Publishes an event by topic ID to all current subscribers, executing
         * their registered callbacks.
         *
         * \param topic The topic ID to publish the event to.
         * \param data  The data to send to all subscriber's callbacks.
         */
        static void publish(std::uint32_t topic, std::shared_ptr<void> data)
        {
            instance->publishI(topic, data);
        };

        /**
         * \brief Subscribes a callback function to the given topic name to be used in future
         * published event handling.
         *
         * \param topic     The topic name to subscribe the callback to.
         * \param consumer  The callback to be invoked for published events of this topic.
         * \return The topic id for the subscribed event name.
         */
        static std::uint32_t subscribe(std::string topic, std::function<void(std::shared_ptr<void>)> consumer)
        {
            return instance->subscribeI(topic, consumer);
        };
    private:
        MessageBroker() = default;

        std::uint32_t publishI(std::string topic, std::shared_ptr<void> data)
        {
            if (topicIds_.find(topic) == topicIds_.end())
            {
                topicIds_.insert(
                        std::pair<std::string, std::uint32_t>{topic, ++lastTopicId_}
                );
            }

            std::uint32_t topicId = topicIds_.at(topic);

            publishI(topicId, data);

            return topicId;
        }

        void publishI(std::uint32_t topic, std::shared_ptr<void> data)
        {
            if (subscribers_.find(topic) != subscribers_.end())
            {
                for (auto& f: subscribers_.at(topic))
                {
                    f(data);
                }
            }
        };

        std::uint32_t subscribeI(std::string topic, std::function<void(std::shared_ptr<void>)> consumer)
        {
            if (topicIds_.find(topic) == topicIds_.end())
            {
                topicIds_.insert(
                        std::pair<std::string, std::uint32_t>{topic, ++lastTopicId_}
                );
            }

            std::uint32_t topicId = topicIds_.at(topic);

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
    private:
        std::unordered_map<std::string, std::uint32_t> topicIds_{};
        std::unordered_map<std::uint32_t, std::vector<std::function<void(std::shared_ptr<void>)>>> subscribers_{};
        std::uint32_t lastTopicId_ = 0;
    };
}