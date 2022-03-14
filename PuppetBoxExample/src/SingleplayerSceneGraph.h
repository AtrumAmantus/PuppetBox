#pragma once

#include <iostream>
#include <memory>
#include <numeric>
#include <queue>
#include <string>
#include <unordered_map>

#include <PuppetBox.h>
#include <puppetbox/Constants.h>
#include <puppetbox/Event.h>
#include <puppetbox/IAnimationCatalogue.h>
#include <puppetbox/KeyCode.h>
#include <puppetbox/SceneObject.h>
#include <puppetbox/UIComponent.h>

#include "AbstractInputProcessor.h"
#include "Command.h"
#include "Common.h"
#include "Constants.h"
#include "InputActions.h"
#include "Entity.h"
#include "EventDef.h"
#include "Game2DInputProcessor.h"
#include "GameEvent.h"
#include "ScreenTranslator.h"
#include "TypeDef.h"
#include "UIAttributeBuilder.h"
#include "UIController.h"
#include "UserInput.h"

namespace Singleplayer
{
    inline static void inputParser(UserInput& userInput)
    {
        if (!userInput.isReading() && !userInput.isEmpty())
        {
            std::string input = userInput.read();
            userInput.clear();

//            std::cout << "You typed: " << input << std::endl;

            auto messageEvent = std::make_shared<UIControllerEvent>();
            messageEvent->action = [&input](UIController& controller) {
                bool error = false;
                auto component = controller.getComponent(CHAT_MESSAGES_BOX, &error);

                if (!error)
                {
                    std::string oldContent = component->getStringAttribute(PB::UI::TEXT_CONTENT).orElse("");
                    component->setStringAttribute(PB::UI::TEXT_CONTENT, oldContent + input + "\n");
                }
            };

            PB::PublishEvent(Event::Topic::UI_TOPIC, messageEvent);

            if (input.substr(0, 7) == "/camera")
            {
                if (input.size() == 7)
                {
                    auto event = std::make_shared<CameraEvent>();
                    event->action = [](PB::Camera& camera) {
                        PB::vec3 p = camera.getPosition();
                        std::cout << "Camera position: " << p.x << ", " << p.y << ", " << p.z << std::endl;
                    };

                    PB::PublishEvent(Event::Topic::CAMERA_TOPIC, event);
                }
                else
                {
                    int v[3]{0, 0, 0};
                    int vi = 0;

                    std::string value;

                    for (std::uint32_t i = 8; vi < 3 && i < input.size(); ++i)
                    {
                        char c = input.c_str()[i];
                        if (c == ' ')
                        {
                            v[vi++] = std::stoi(value);
                            value = "";
                        }
                        else
                        {
                            value += c;
                        }
                    }

                    if (vi < 3 && !value.empty())
                    {
                        v[vi++] = std::stoi(value);
                        value = "";
                    }

                    std::cout << "Moving camera: " << v[0] << ", " << v[1] << ", " << v[2] << std::endl;
                    auto event = std::make_shared<CameraEvent>();
                    event->action = [v](PB::Camera& camera) {
                        camera.moveTo({static_cast<float>(v[0]), static_cast<float>(v[1]), static_cast<float>(v[2])});
                    };

                    PB::PublishEvent(Event::Topic::CAMERA_TOPIC, event);
                }
            }
            else if (input == "/horizontal")
            {
                auto event = std::make_shared<UIControllerEvent>();
                event->action = [](UIController& controller) {
                    bool error = false;

                    auto component = controller.getComponent(CHAT_BOX, &error);

                    if (!error)
                    {
                        component->setUIntAttribute(PB::UI::LAYOUT, PB::UI::Layout::HORIZONTAL);
                    }
                };

                PB::PublishEvent(Event::Topic::UI_TOPIC, event);
            }
            else if (input == "/vertical")
            {
                auto event = std::make_shared<UIControllerEvent>();
                event->action = [](UIController& controller) {
                    bool error = false;

                    auto component = controller.getComponent(CHAT_BOX, &error);

                    if (!error)
                    {
                        component->setUIntAttribute(PB::UI::LAYOUT, PB::UI::Layout::VERTICAL);
                    }
                };

                PB::PublishEvent(Event::Topic::UI_TOPIC, event);
            }
            else if (input == "/ortho")
            {
                auto event = std::make_shared<ViewModeEvent>();
                event->mode = PB::SceneView::ORTHO;

                PB::PublishEvent(Event::Topic::VIEW_MODE_TOPIC, event);
            }
            else if (input == "/perspective" || input == "/persp")
            {
                auto event = std::make_shared<ViewModeEvent>();
                event->mode = PB::SceneView::PERSPECTIVE;

                PB::PublishEvent(Event::Topic::VIEW_MODE_TOPIC, event);
            }
            else if (input.substr(0, 7) == "/lspace")
            {
                float spaceMultiplier = std::stof(input.substr(8));

                auto event = std::make_shared<UIControllerEvent>();
                event->action = [&spaceMultiplier](UIController& controller) {
                    bool error = false;

                    auto component = controller.getComponent(CHAT_MESSAGES_BOX, &error);

                    if (!error)
                    {
                        component->setFloatAttribute(PB::UI::LETTER_SPACE, spaceMultiplier);
                    }
                };

                PB::PublishEvent(Event::Topic::UI_TOPIC, event);
            }
            else if (input.substr(0, 7) == "/wspace")
            {
                float spaceMultiplier = std::stof(input.substr(8));

                auto event = std::make_shared<UIControllerEvent>();
                event->action = [&spaceMultiplier](UIController& controller) {
                    bool error = false;

                    auto component = controller.getComponent(CHAT_MESSAGES_BOX, &error);

                    if (!error)
                    {
                        component->setFloatAttribute(PB::UI::WORD_SPACE, spaceMultiplier);
                    }
                };

                PB::PublishEvent(Event::Topic::UI_TOPIC, event);
            }
            else if (input.substr(0, 5) == "/wrap")
            {
                bool enabled = input.substr(6) == "true";

                auto event = std::make_shared<UIControllerEvent>();
                event->action = [&enabled](UIController& controller) {
                    bool error = false;

                    auto component = controller.getComponent(CHAT_MESSAGES_BOX, &error);

                    if (!error)
                    {
                        component->setBoolAttribute(PB::UI::WORD_WRAP, enabled);
                    }
                };

                PB::PublishEvent(Event::Topic::UI_TOPIC, event);
            }
            else if (input == "/multi")
            {
                PB::SetActiveScene("Multiplayer");
            }
            else if (input == "/play walk")
            {
                auto event = std::make_shared<UpdateEntityEvent>();
                event->action = [](Entity* entity) {
                    entity->playAnimation(Constants::Animation::kWalk, 0);
                };

                PB::PublishEvent(Event::Topic::UPDATE_ENTITY_TOPIC, event);
            }
            else if (input == "/play idle0")
            {
                auto event = std::make_shared<UpdateEntityEvent>();
                event->action = [](Entity* entity) {
                    entity->playAnimation(Constants::Animation::kIdle0, 0);
                };

                PB::PublishEvent(Event::Topic::UPDATE_ENTITY_TOPIC, event);
            }
            else if (input == "/stop")
            {
                auto event = std::make_shared<UpdateEntityEvent>();
                event->action = [](Entity* entity) {
                    entity->stopAnimation();
                };

                PB::PublishEvent(Event::Topic::UPDATE_ENTITY_TOPIC, event);
            }
            else if (input.substr(0, 5) == "/bone")
            {
                std::uint32_t i = 6;
                std::string boneName = "";

                PB::vec3 rotation{};

                while (i < input.size() && input.c_str()[i] != ' ')
                {
                    boneName += input.c_str()[i++];
                }

                i++;

                std::string value = "";

                while (i < input.size() && input.c_str()[i] != ' ')
                {
                    value += input.c_str()[i++];
                }

                i++;

                if (!value.empty())
                {
                    rotation.x = std::stof(value);
                    value = "";

                    while (i < input.size() && input.c_str()[i] != ' ')
                    {
                        value += input.c_str()[i++];
                    }

                    i++;

                    if (!value.empty())
                    {
                        rotation.y = std::stof(value);
                        value = "";

                        while (i < input.size() && input.c_str()[i] != ' ')
                        {
                            value += input.c_str()[i++];
                        }

                        if (!value.empty())
                        {
                            rotation.z = std::stof(value);
                        }
                    }
                }

                auto event = std::make_shared<UpdateEntityEvent>();

                event->action = [boneName, rotation](Entity* entity) {
                    entity->rotateBone(boneName, rotation);
                };

                PB::PublishEvent(Event::Topic::UPDATE_ENTITY_TOPIC, event);
            }
        }
        else
        {
            userInput.component()->setStringAttribute(PB::UI::TEXT_CONTENT, userInput.read());
        }
    }
}

class SingleplayerSceneGraph : public PB::AbstractSceneGraph
{
public:
    SingleplayerSceneGraph(const std::string& sceneName) : PB::AbstractSceneGraph(sceneName) {};

protected:
    bool setUps() override
    {
        setViewMode(PB::SceneView::ORTHO);

        camera().setPanSpeed(100.0f);
        camera().setZoomSpeed(2.0f);

        bool success = true;

        if (PB::LoadAssetPack("Assets1"))
        {
            eventSubscriptions();

            inputActions_ = InputActions{input()};

            controlRegistration(inputActions_);

            camera().moveTo({0.0f, 0.0f, 3.0f});

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

        return success;
    }

    void preLoopUpdates(float deltaTime) override
    {
        player_ = playerToControl_;

        if (wasClearSceneInvoked())
        {
            player_ = nullptr;
            playerToControl_ = nullptr;
        }

        updateFrameCounter(deltaTime);

        Singleplayer::inputParser(userInput_);

        uiController_.update(deltaTime);
    }

    void postLoopUpdates(float deltaTime) override
    {
        if (player_ != nullptr && (player_->moveVector.x != 0 || player_->moveVector.y != 0))
        {
            camera().moveNear(
                    {player_->position.x, player_->position.y, 0.0f},
                    {100.0f, 100.0f, 0.0f}
            );
        }
    }

    void renders() const override
    {
        uiController_.render();
    }

    void processInputs() override
    {
        screenTranslator_.cursor.renderCoords = {input()->mouse.x, (*renderWindow().height) - input()->mouse.y};
        screenTranslator_.cursor.worldCoords = (camera().calculateInverseViewMatrix(renderWindow(), viewMode()) * PB::vec4 {
                static_cast<float>(screenTranslator_.cursor.renderCoords.x),
                static_cast<float>(screenTranslator_.cursor.renderCoords.y),
                0.0f,
                1.0f
        }).vec3();
        screenTranslator_.cursor.worldVector = {};
        inputProcessor_->processInput();
    }

private:
    UIController uiController_{};
    UserInput userInput_{};
    InputActions inputActions_{};
    Entity* player_ = nullptr;
    Entity* playerToControl_ = nullptr;
    AbstractInputProcessor* inputProcessor_ = nullptr;
    ScreenTranslator screenTranslator_{};
    std::queue<PB::UUID> subscriptions_{};

private:
    /**
     * \brief Sets up all the required events for the scene.  Events are permanent once they are registered,
     * and can be shared between scenes.
     */
    void eventSubscriptions()
    {
        PB::UUID uuid;

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

            event->action(camera());
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

        defaultSceneSetup();

        return success;
    }

    bool defaultSceneSetup()
    {
        bool success = true;

        playerToControl_ = new Entity{};

        if (PB::CreateSceneObject("Assets1/Sprites/GenericMob", playerToControl_))
        {
            playerToControl_->name = "Fred";
            playerToControl_->position = PB::vec3{150.0f, 50.0f, -50.0f};
            //            myEntity->setBehavior(PB::AI::Behavior::WANDER);
            addSceneObject(playerToControl_);
        }

        PB::BoneMap boneMap = playerToControl_->getBones();

        //TODO: These load times are long, separate thread and maybe some optimizations
        PB::PreloadAnimationFrames(Constants::Animation::kIdle0, boneMap);
        PB::PreloadAnimationFrames(Constants::Animation::kWalk, boneMap);

        auto* weapon = new Entity();

        if (PB::CreateSceneObject("Assets1/Sprites/Weapons/Knife", weapon))
        {
            weapon->name = "weapon";
            weapon->position = {0.0f, 0.0f, 0.0f};
            addSceneObject(weapon);
            weapon->attachTo(playerToControl_, "weapon_attach_right");
        }

        auto* chain = new Entity();

        if (PB::CreateSceneObject("Assets1/Sprites/Misc/Chain", chain))
        {
            chain->name = "chain";
            chain->position = {0.0f, 0.0f, -40.0f};
            addSceneObject(chain);
        }

        return success;
    };
};