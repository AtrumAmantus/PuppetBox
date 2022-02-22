#pragma once

#include <iostream>
#include <memory>
#include <numeric>
#include <string>
#include <unordered_map>

#include <PuppetBox.h>
#include <puppetbox/Constants.h>
#include <puppetbox/Event.h>
#include <puppetbox/IAnimationCatalogue.h>
#include <puppetbox/KeyCode.h>
#include <puppetbox/SceneObject.h>

#include "AbstractInputProcessor.h"
#include "Constants.h"
#include "InputActions.h"
#include "Entity.h"
#include "EventDef.h"
#include "Game2DInputProcessor.h"
#include "UIAttributeBuilder.h"
#include "UIController.h"
#include "UserInput.h"

#define INPUT_BOX 0
#define FPS_BOX 1

#define FPS_MAX_FRAME_COUNT 60

namespace
{
    float _timeSinceGfxFpsCheck = 0.0f;
    std::uint32_t _frameIndex = 0;
    float _frameRates[FPS_MAX_FRAME_COUNT]{};

    inline std::uint32_t calculateAverageFps(float frameTimes[], std::uint8_t frameCount)
    {
        float sum = 0;

        for (std::uint8_t i = 0; i < frameCount; ++i)
        {
            sum += frameTimes[i];
        }

        float average = sum / frameCount;

        return 1.0f / average;
    }

    inline void controlRegistration(InputActions& controls)
    {
        controls.registerCommand(InputActions::CAMERA_UP, KEY_UP);
        controls.registerCommand(InputActions::CAMERA_DOWN, KEY_DOWN);
        controls.registerCommand(InputActions::CAMERA_LEFT, KEY_LEFT);
        controls.registerCommand(InputActions::CAMERA_RIGHT, KEY_RIGHT);
        controls.registerCommand(InputActions::FORWARD, KEY_W);
        controls.registerCommand(InputActions::BACKWARD, KEY_S);
        controls.registerCommand(InputActions::LEFT, KEY_A);
        controls.registerCommand(InputActions::RIGHT, KEY_D);
        controls.registerCommand(InputActions::QUIT, KEY_ESCAPE);
    }

    inline bool updateFrameCounter(float deltaTime)
    {
        bool error = false;

        _frameRates[_frameIndex] = deltaTime;
        _timeSinceGfxFpsCheck += deltaTime;
        _frameIndex = (_frameIndex + 1) % FPS_MAX_FRAME_COUNT;

        if (_timeSinceGfxFpsCheck > 0.1)
        {
            _timeSinceGfxFpsCheck -= 0.25f;
            std::uint32_t averageFps = calculateAverageFps(_frameRates, FPS_MAX_FRAME_COUNT);

            std::shared_ptr<UIControllerEvent> event = std::make_shared<UIControllerEvent>();
            event->action = [averageFps](UIController& controller) {
                bool error = false;
                auto component = controller.getComponent(FPS_BOX, &error);

                if (!error)
                {
                    component->setStringAttribute(PB::UI::TEXT_CONTENT, std::to_string(averageFps) + " FPS");
                }
            };

            PB::PublishEvent(Event::Topic::UI_TOPIC, event);
        }
        return error;
    }

    inline bool uiSetup(UIController& uiController, UserInput& userInput)
    {
        bool error = false;

        auto textBox = std::shared_ptr<PB::UIComponent>(
                PB::CreateUIComponent(
                        PB::UI::TEXT_AREA,
                        std::move(UIAttributeBuilder{}
                                          .dimensions({400, 200})
                                          .fontSize(16)
                                          .fontType(Constants::Font::kMochiyPop)
                                          .build()
                        ),
                        &error
                )
        );

        textBox->setStringAttribute(PB::UI::TEXT_CONTENT,
                                    "This is a long sentence that should wrap to the next line, followed by some additional words to check to see how big the bounding box is, and if it is properly clipping the text.");

        auto inputBox = std::shared_ptr<PB::UIComponent>(
                PB::CreateUIComponent(
                        PB::UI::TEXT_AREA,
                        std::move(UIAttributeBuilder{}
                                          .dimensions({400, 24})
                                          .fontSize(16)
                                          .fontType(Constants::Font::kMochiyPop)
                                          .build()
                        ),
                        &error
                )
        );

        userInput.targetComponent(inputBox);

        auto groupComponent = std::shared_ptr<PB::UIComponent>(
                PB::CreateUIComponent(
                        PB::UI::GROUP,
                        std::move(UIAttributeBuilder{}
                                          .origin(PB::UI::Origin::BOTTOM_LEFT)
                                          .position(PB::vec3{10, 10, 1})
                                          .layout(PB::UI::Layout::VERTICAL)
                                          .build()
                        ),
                        &error
                )
        );

        groupComponent->addComponent(textBox);
        groupComponent->addComponent(inputBox);

        uiController.addComponent(groupComponent, INPUT_BOX);

        auto fpsCounter = std::shared_ptr<PB::UIComponent>(
                PB::CreateUIComponent(
                        PB::UI::TEXT_AREA,
                        std::move(UIAttributeBuilder{}
                                          .origin(PB::UI::Origin::TOP_LEFT)
                                          .fontSize(18)
                                          .fontType(Constants::Font::kMochiyPop)
                                          .dimensions(PB::vec2{100, 24})
                                          .position(PB::vec3{10, 590, 1})
                                          .build()),
                        &error
                )
        );

        uiController.addComponent(fpsCounter, FPS_BOX);

        return !error;
    }

    inline void inputParser(UserInput& userInput)
    {
        if (!userInput.isReading() && !userInput.isEmpty())
        {
            std::string input = userInput.read();
            userInput.clear();

            std::cout << "You typed: " << input << std::endl;

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
            else
            {
                if (input == "/horizontal")
                {
                    auto event = std::make_shared<UIControllerEvent>();
                    event->action = [](UIController& controller) {
                        bool error = false;

                        auto component = controller.getComponent(INPUT_BOX, &error);

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

                        auto component = controller.getComponent(INPUT_BOX, &error);

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
                else if (input == "/play walk")
                {
                    auto event = std::make_shared<PlayerEvent>();
                    event->action = [](Entity& entity) {
                        entity.playAnimation(Constants::Animation::kWalk, 0);
                    };

                    PB::PublishEvent(Event::Topic::PLAYER_TOPIC, event);
                }
                else if (input == "/play idle0")
                {
                    auto event = std::make_shared<PlayerEvent>();
                    event->action = [](Entity& entity) {
                        entity.playAnimation(Constants::Animation::kIdle0, 0);
                    };

                    PB::PublishEvent(Event::Topic::PLAYER_TOPIC, event);
                }
                else if (input == "/stop")
                {
                    auto event = std::make_shared<PlayerEvent>();
                    event->action = [](Entity& entity) {
                        entity.stopAnimation();
                    };

                    PB::PublishEvent(Event::Topic::PLAYER_TOPIC, event);
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

                    auto event = std::make_shared<PlayerEvent>();
                    event->action = [boneName, rotation](Entity& entity) {
                        entity.rotateBone(boneName, rotation);
                    };

                    PB::PublishEvent(Event::Topic::PLAYER_TOPIC, event);
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
                else if (input.substr(0, 5) == "/send")
                {
                    std::string message = input.substr(6);

                    auto chatEvent = std::make_shared<UserChatEvent>();
                    chatEvent->message = message;

                    PB::PublishEvent(Event::Topic::USER_CHAT_TOPIC, chatEvent);
                }
            }
        }
        else
        {
            userInput.component()->setStringAttribute(PB::UI::TEXT_CONTENT, userInput.read());
        }
    }
}


class CustomSceneHandler : public PB::AbstractSceneGraph
{
public:
    CustomSceneHandler(const std::string& sceneName) : PB::AbstractSceneGraph(sceneName) {};

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

    void updates(float deltaTime) override
    {
        updateFrameCounter(deltaTime);

        inputParser(userInput_);

        uiController_.update(deltaTime);
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
    Entity* player = nullptr;
    AbstractInputProcessor* inputProcessor_ = nullptr;
    bool areListenersInitialized_ = false;

private:
    /**
     * \brief Sets up all the required events for the scene.  Events are permanent once they are registered,
     * and can be shared between scenes.
     *
     * TODO: common scene event setup?
     */
    void eventSubscriptions()
    {
        // Listen for network thread ready event to start registering listeners
        Event::Topic::NETWORK_TOPIC = PB::SubscribeEvent(PB_EVENT_NETWORK, [this](std::shared_ptr<void> event) {
            auto networkEvent = std::static_pointer_cast<PB::NetworkEvent>(event);

            if (networkEvent->type == PB::Event::READY && !areListenersInitialized_)
            {
                areListenersInitialized_ = true;
                // Network Listener Events
                Event::Topic::USER_CHAT_TOPIC = PB::RegisterTopic(PBEX_EVENT_USER_CHAT);
                PB::RegisterNetworkEventListener(
                        PBEX_EVENT_USER_CHAT,
                        [](std::shared_ptr<void> data, std::uint8_t** dataOut, std::uint32_t* dataLength) {
                            auto chatEvent = std::static_pointer_cast<UserChatEvent>(data);

                            // +4 bytes for event id
                            *dataLength = chatEvent->message.size() + 4;
                            *dataOut = new uint8_t[*dataLength];
                            (*dataOut)[0] = 0;
                            (*dataOut)[1] = 0;
                            (*dataOut)[2] = 0;
                            (*dataOut)[3] = 4;
                            for (std::uint32_t i = 0; i < chatEvent->message.size(); ++i) {
                                (*dataOut)[i + 4] = chatEvent->message.c_str()[i];
                            }
                        });
            }
        });

        // Ping the network thread in case it already started (in which case we missed the ready pulse)
        auto networkEvent = std::make_shared<PB::NetworkEvent>();
        networkEvent->type = PB::Event::READY_CHECK;
        PB::PublishEvent(Event::Topic::NETWORK_TOPIC, networkEvent);

        // Application Events
        Event::Topic::UI_TOPIC = PB::SubscribeEvent(PBEX_EVENT_UI, [this](std::shared_ptr<void> data) {
            std::shared_ptr<UIControllerEvent> uiEvent = std::static_pointer_cast<UIControllerEvent>(data);

            uiEvent->action(uiController_);
        });

        Event::Topic::TERMINATE_TOPIC = PB::SubscribeEvent(
                PBEX_EVENT_TERMINATE_APP,
                [this](std::shared_ptr<void> data) { input()->window.windowClose = true; });

        Event::Topic::CAMERA_TOPIC = PB::SubscribeEvent(PBEX_EVENT_CAMERA, [this](std::shared_ptr<void> data) {
            auto event = std::static_pointer_cast<CameraEvent>(data);

            event->action(getCamera());
        });

        Event::Topic::PLAYER_TOPIC = PB::SubscribeEvent(PBEX_EVENT_PLAYER, [this](std::shared_ptr<void> data) {
            auto event = std::static_pointer_cast<PlayerEvent>(data);

            event->action(*player);

            getCamera().moveNear(
                    {player->position.x, player->position.y, 0.0f},
                    {100.0f, 100.0f, 0.0f}
            );
        });

        Event::Topic::VIEW_MODE_TOPIC = PB::SubscribeEvent(PBEX_EVENT_VIEW_MODE, [this](std::shared_ptr<void> data) {
            auto event = std::static_pointer_cast<ViewModeEvent>(data);

            setViewMode(event->mode);
        });
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

        player = new Entity{};

        if (PB::CreateSceneObject("Assets1/Sprites/GenericMob", player))
        {
            player->name = "Fred";
            player->position = PB::vec3{150.0f, 50.0f, -50.0f};
            //            myEntity->setBehavior(PB::AI::Behavior::WANDER);
            addSceneObject(player);
        }

        auto* weapon = new Entity();

        if (PB::CreateSceneObject("Assets1/Sprites/Weapons/Knife", weapon))
        {
            weapon->name = "weapon";
            weapon->position = {0.0f, 0.0f, 0.0f};
            addSceneObject(weapon);
            weapon->attachTo(player, "weapon_attach_right");
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