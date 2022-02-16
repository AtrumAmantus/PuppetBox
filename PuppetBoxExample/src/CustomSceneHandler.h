#pragma once

#include <iostream>
#include <memory>
#include <numeric>
#include <string>
#include <unordered_map>

#include <PuppetBox.h>
#include <puppetbox/Constants.h>
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
                                          .dimensions({300, 200})
                                          .fontSize(24)
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
                                          .dimensions({200, 24})
                                          .fontSize(24)
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

            if (input.substr(0, 7) == "/engine")
            {
                PB::PublishEvent("pb_test_event", std::make_shared<std::string>(input.substr(8)));
            }
            else if (input.substr(0, 7) == "/camera")
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

                    while(i < input.size() && input.c_str()[i] != ' ')
                    {
                        value += input.c_str()[i++];
                    }

                    i++;

                    if (!value.empty())
                    {
                        rotation.x = std::stof(value);
                        value = "";

                        while(i < input.size() && input.c_str()[i] != ' ')
                        {
                            value += input.c_str()[i++];
                        }

                        i++;

                        if (!value.empty())
                        {
                            rotation.y = std::stof(value);
                            value = "";

                            while(i < input.size() && input.c_str()[i] != ' ')
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
            }
        }
        else
        {
            userInput.component()->setStringAttribute(PB::UI::TEXT_CONTENT, userInput.read());
        }
    }
}


class CustomSceneHandler : public PB::AbstractSceneHandler
{
public:
    bool setUp() override
    {
        setViewMode(PB::SceneView::ORTHO);

        getCamera().setPanSpeed(100.0f);
        getCamera().setZoomSpeed(2.0f);

        bool success = true;

        Event::Topic::UI_TOPIC = PB::SubscribeEvent("pbex_ui_update", [this](std::shared_ptr<void> data){
            std::shared_ptr<UIControllerEvent> uiEvent = std::static_pointer_cast<UIControllerEvent>(data);

            uiEvent->action(uiController_);
        });

        Event::Topic::TERMINATE_TOPIC = PB::SubscribeEvent("pbex_terminate_app", [this](std::shared_ptr<void> data){ input()->window.windowClose = true;});

        Event::Topic::CAMERA_TOPIC = PB::SubscribeEvent("pbex_camera_update", [this](std::shared_ptr<void> data){
            auto event = std::static_pointer_cast<CameraEvent>(data);

            event->action(getCamera());
        });

        Event::Topic::PLAYER_TOPIC = PB::SubscribeEvent("pbex_player_update", [this](std::shared_ptr<void> data) {
            auto event = std::static_pointer_cast<PlayerEvent>(data);

            event->action(*player);

            getCamera().moveNear(
                    {player->position.x, player->position.y, 0.0f},
                    {100.0f, 100.0f, 0.0f}
            );
        });

        Event::Topic::VIEW_MODE_TOPIC = PB::SubscribeEvent("pbex_view_mode_update", [this](std::shared_ptr<void> data) {
            auto event = std::static_pointer_cast<ViewModeEvent>(data);

            setViewMode(event->mode);
        });

        inputActions_ = InputActions{input()};

        controlRegistration(inputActions_);

        getCamera().moveTo({0.0f, 0.0f, 3.0f});

        inputProcessor_ = new Game2DInputProcessor(userInput_, inputActions_, *input());

        if (PB::LoadAnimationsPack(Constants::Animation::Pack::kBasicHuman))
        {
            if (PB::LoadFontAsset(Constants::Font::kMochiyPop, 72))
            {
                if (!uiSetup(uiController_, userInput_))
                {
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
            }
            else
            {
                success = false;
                std::cout << "Failed to load font asset" << std::endl;
            }
        }
        else
        {
            std::cout << "Failed to load animation pack" << std::endl;
            success = false;
        }

        return success;
    }

protected:
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
    InputActions inputActions_;
    Entity* player = nullptr;
    AbstractInputProcessor* inputProcessor_ = nullptr;
};