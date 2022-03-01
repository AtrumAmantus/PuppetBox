#pragma once

#include <puppetbox/AbstractSceneGraph.h>

#include "AbstractInputProcessor.h"
#include "Common.h"
#include "Constants.h"
#include "Entity.h"
#include "Game2DInputProcessor.h"
#include "GameEvent.h"
#include "InputActions.h"
#include "NetworkEvents.h"
#include "UIController.h"
#include "UserInput.h"

namespace Multiplayer
{
    inline static void inputParser(UserInput& userInput)
    {
        if (!userInput.isReading() && !userInput.isEmpty())
        {
            std::string input = userInput.read();
            userInput.clear();

            std::cout << "You typed: " << input << std::endl;

            if (input == "/single")
            {
                PB::SetActiveScene("Singleplayer");
            }
            else if (input.substr(0, 8) == "/connect")
            {
                std::uint32_t substringIndexes = 0;
                std::uint8_t byteIndex = 0;
                std::uint8_t count = 0;

                for (std::uint32_t i = 8; i < input.size(); ++i)
                {
                    if (input[i] == ' ')
                    {
                        if (count != 0)
                        {
                            substringIndexes |= count << (byteIndex++ * 8);
                            count = 0;
                        }
                    }
                    else
                    {
                        if (count == 0)
                        {
                            substringIndexes |= i << (byteIndex++ * 8);
                        }

                        ++count;
                    }
                }

                // If the string ended, check if we even finished indexing the port
                if (count != 0)
                {
                    substringIndexes |= count << (byteIndex * 8);
                }

                std::uint8_t* indexes = (std::uint8_t*) &substringIndexes;
                std::string host = input.substr(indexes[0], indexes[1]);
                std::string portString = input.substr(indexes[2], indexes[3]);
                std::uint16_t port = std::stoi(portString);

                auto networkEvent = std::make_shared<PB::NetworkEvent>();
                networkEvent->type = PB::Event::NetworkEventType::CONNECT;
                networkEvent->host = host;
                networkEvent->port = port;

                PB::PublishEvent(Event::Topic::NETWORK_TOPIC, networkEvent);
            }
            else if (input == "/disconnect")
            {
                auto networkEvent = std::make_shared<PB::NetworkEvent>();
                networkEvent->type = PB::Event::DISCONNECT;

                PB::PublishEvent(Event::Topic::NETWORK_TOPIC, networkEvent);
            }
            else if (input.substr(0, 5) == "/say")
            {
                std::string message = input.substr(6);

                auto chatEvent = std::make_shared<UserChatEvent>();
                chatEvent->message = message;

                PB::PublishEvent(Event::Topic::USER_CHAT_TOPIC, chatEvent);
            }
        }
        else
        {
            userInput.component()->setStringAttribute(PB::UI::TEXT_CONTENT, userInput.read());
        }
    }
}

class MultiplayerSceneGraph : public PB::AbstractSceneGraph
{
public:
    MultiplayerSceneGraph(const std::string& sceneName) : PB::AbstractSceneGraph(sceneName)
    {};

protected:
    bool setUps() override
    {
        setViewMode(PB::SceneView::ORTHO);

        getCamera().setPanSpeed(100.0f);
        getCamera().setZoomSpeed(2.0f);

        bool success = true;

        if (PB::LoadAssetPack("Assets1"))
        {
            eventSubscriptions();

            inputActions_ = InputActions{input()};

            controlRegistration(inputActions_);

            getCamera().moveTo({0.0f, 0.0f, 3.0f});

            inputProcessor_ = new Game2DInputProcessor(userInput_, inputActions_, input());

            if (PB::LoadAnimationsPack(Constants::Animation::Pack::kBasicHuman))
            {
                if (PB::LoadFontAsset(Constants::Font::kMochiyPop, 72))
                {
                    sceneSetUp();
                }
                else
                {
                    success = false;
                    std::cout << "Failed to load font asset" << std::endl;
                }
            }
            else
            {
                success = false;
                std::cout << "Failed to load animation pack" << std::endl;
            }
        }
        else
        {
            success = false;
            std::cout << "Failed to load assets" << std::endl;
        }

        return success;
    }

    bool tearDowns() override
    {
        bool success = true;

        clearSceneNow();
        uiController_.clear();

        while (!subscriptions_.empty())
        {
            PB::Unsubscribe(subscriptions_.front());
            subscriptions_.pop();
        }

        auto event = std::make_shared<PB::NetworkEvent>();
        event->type = PB::Event::DISCONNECT;
        PB::PublishEvent(PB_EVENT_NETWORK, event);

        return success;
    }

    void preLoopUpdates(float deltaTime) override
    {
        if (playerToControl_ != PB::UUID::nullUUID())
        {
            std::cout << "Setting player to UUID" << std::endl;
            player_ = (Entity*) getSceneObject(playerToControl_);

            // Lazy way to handle race condition where player is set before entity was added
            if (player_ != nullptr)
            {
                playerToControl_ = PB::UUID::nullUUID();
            }
        }

        if (wasClearSceneInvoked())
        {
            player_ = nullptr;
            playerToControl_ = PB::UUID::nullUUID();
        }

        updateFrameCounter(deltaTime);

        Multiplayer::inputParser(userInput_);

        uiController_.update(deltaTime);
    }

    void postLoopUpdates(float deltaTime) override
    {
        if (player_ != nullptr && (player_->moveVector.x != 0 || player_->moveVector.y != 0))
        {
            getCamera().moveNear(
                    {player_->position.x, player_->position.y, 0.0f},
                    {100.0f, 100.0f, 0.0f}
            );
        }

        timeSinceLastLocationUpdate_ += deltaTime;

        if (networkConnected() && timeSinceLastLocationUpdate_ > 1.0f)
        {
            timeSinceLastLocationUpdate_ = fmod(timeSinceLastLocationUpdate_, 1.0f);

            if (player_ != nullptr)
            {
                auto event = std::make_shared<PlayerLocationEvent>();
                event->location = player_->position;
                PB::PublishEvent(Event::Topic::PLAYER_LOC_TOPIC, event);
            }
        }
    }

    void renders() const override
    {
        uiController_.render();
    }

    void processInputs() override
    {
        inputProcessor_->processInput();
    }

private:
    UIController uiController_{};
    UserInput userInput_{};
    InputActions inputActions_{};
    Entity* player_ = nullptr;
    PB::UUID playerToControl_{};
    AbstractInputProcessor* inputProcessor_ = nullptr;
    float timeSinceLastLocationUpdate_ = 0.0f;
    std::queue<PB::UUID> subscriptions_{};

private:
    /**
     * \brief Sets up all the required events for the scene.  Events are permanent once they are registered,
     * and can be shared between scenes.
     */
    void eventSubscriptions()
    {
        Event::Topic::NETWORK_TOPIC = PB::RegisterTopic(PB_EVENT_NETWORK);

        // Listen for network thread ready event to start registering listeners
        Event::Topic::NETWORK_STATUS_TOPIC = PB::RegisterTopic(PB_EVENT_NETWORK_STATUS);
        auto uuid = PB::SubscribeEvent(PB_EVENT_NETWORK_STATUS, &networkReadyStatusEvent);

        subscriptions_.push(uuid);

        // Ping the network thread in case it already started (in which case we missed the ready pulse)
        auto networkEvent = std::make_shared<PB::NetworkEvent>();
        networkEvent->type = PB::Event::NetworkEventType::READY_CHECK;
        PB::PublishEvent(Event::Topic::NETWORK_TOPIC, networkEvent);

        // Application Events
        Event::Topic::UI_TOPIC = PB::RegisterTopic(PBEX_EVENT_UI);
        uuid = PB::SubscribeEvent(PBEX_EVENT_UI, [this](std::shared_ptr<void> data) {
            std::shared_ptr<UIControllerEvent> uiEvent = std::static_pointer_cast<UIControllerEvent>(data);

            uiEvent->action(uiController_);
        });

        subscriptions_.push(uuid);

        Event::Topic::TERMINATE_TOPIC = PB::RegisterTopic(PBEX_EVENT_TERMINATE_APP);
        uuid = PB::SubscribeEvent(
                PBEX_EVENT_TERMINATE_APP,
                [this](std::shared_ptr<void> data) { input()->window.windowClose = true; });

        subscriptions_.push(uuid);

        Event::Topic::CAMERA_TOPIC = PB::RegisterTopic(PBEX_EVENT_CAMERA);
        uuid = PB::SubscribeEvent(PBEX_EVENT_CAMERA, [this](std::shared_ptr<void> data) {
            auto event = std::static_pointer_cast<CameraEvent>(data);

            event->action(getCamera());
        });

        subscriptions_.push(uuid);

        Event::Topic::PLAYER_TOPIC = PB::RegisterTopic(PBEX_EVENT_PLAYER);
        uuid = PB::SubscribeEvent(PBEX_EVENT_PLAYER, [this](std::shared_ptr<void> data) {
            //TODO: Would be better to simply unsubscribe from this event if no player_ model exists
            if (player_ != nullptr)
            {
                auto event = std::static_pointer_cast<PlayerEvent>(data);

                for (std::uint32_t i = 0; i < event->commandCount; ++i)
                {
                    player_->setCommandState(event->commandStates[i]);
                }
            }
        });

        subscriptions_.push(uuid);

        Event::Topic::VIEW_MODE_TOPIC = PB::RegisterTopic(PBEX_EVENT_VIEW_MODE);
        uuid = PB::SubscribeEvent(PBEX_EVENT_VIEW_MODE, [this](std::shared_ptr<void> data) {
            auto event = std::static_pointer_cast<ViewModeEvent>(data);

            setViewMode(event->mode);
        });

        subscriptions_.push(uuid);

        Event::Topic::CREATE_ENTITY_TOPIC = PB::RegisterTopic(PBEX_EVENT_CREATE_ENTITY);
        uuid = PB::SubscribeEvent(PBEX_EVENT_CREATE_ENTITY, [this](std::shared_ptr<void> data) {
            auto createEntityEvent = std::static_pointer_cast<CreateEntityEvent>(data);

            auto entity = new Entity{};

            if (PB::CreateSceneObject("Assets1/Sprites/GenericMob", entity, createEntityEvent->uuid))
            {
                entity->name = "Fred";
                entity->position = createEntityEvent->position;
                addSceneObject(entity);
            }
        });

        subscriptions_.push(uuid);

        Event::Topic::DESTROY_ENTITY_TOPIC = PB::RegisterTopic(PBEX_EVENT_DESTROY_ENTITY);
        uuid = PB::SubscribeEvent(PBEX_EVENT_DESTROY_ENTITY, [this](std::shared_ptr<void> data) {
            auto destroyEntityEvent = std::static_pointer_cast<DestroyEntityEvent>(data);

            removeSceneObject(destroyEntityEvent->uuid);
        });

        subscriptions_.push(uuid);

        Event::Topic::SET_USER_ENTITY_TOPIC = PB::RegisterTopic(PBEX_EVENT_SET_USER_ENTITY);
        uuid = PB::SubscribeEvent(PBEX_EVENT_SET_USER_ENTITY, [this](std::shared_ptr<void> data) {
            auto setUserEntityEvent = std::static_pointer_cast<SetUserEntityEvent>(data);

            std::cout << "Setting PlayerToControl UUID" << std::endl;
            playerToControl_ = setUserEntityEvent->uuid;
        });

        subscriptions_.push(uuid);

        Event::Topic::ENTITY_UPDATE_ACTION_TOPIC = PB::RegisterTopic(PBEX_EVENT_UPDATE_ENTITY_ACTION);
        uuid = PB::SubscribeEvent(PBEX_EVENT_UPDATE_ENTITY_ACTION, [this](std::shared_ptr<void> data) {
            auto updateEntityEvent = std::static_pointer_cast<UpdateEntityActionEvent>(data);

            ((Entity*) getSceneObject(updateEntityEvent->uuid))->setCommandState(updateEntityEvent->commandState);
        });

        subscriptions_.push(uuid);

        Event::Topic::ENTITY_UPDATE_LOCATION_TOPIC = PB::RegisterTopic(PBEX_EVENT_UPDATE_ENTITY_LOC);
        uuid = PB::SubscribeEvent(PBEX_EVENT_UPDATE_ENTITY_LOC, [this](std::shared_ptr<void> data) {
            auto updateEntityEvent = std::static_pointer_cast<UpdateEntityLocationEvent>(data);

            ((Entity*) getSceneObject(updateEntityEvent->uuid))->setPosition(updateEntityEvent->location);
        });

        subscriptions_.push(uuid);
    };

    /**
     * \brief Setup logic required to run before the current scene can be utilized.
     *
     * \return True if the scene was set up successfully, False otherwise.
     */
    bool sceneSetUp()
    {
        bool success = true;

        if (!uiSetup(uiController_, userInput_))
        {
            success = false;
            std::cout << "Failed to load interface" << std::endl;
        }

        return success;
    }

};