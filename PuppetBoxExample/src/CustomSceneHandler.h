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

#include "Constants.h"
#include "Controls.h"
#include "Entity.h"
#include "UIAttributeBuilder.h"
#include "UIController.h"
#include "UserInput.h"

#define INPUT_BOX 0
#define FPS_BOX 1

namespace
{
    float timeSinceFpsCheck = 0.0f;

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
}

class CustomSceneHandler : public PB::AbstractSceneHandler
{
public:
    void setUp() override
    {
        setViewMode(PB::SceneView::ORTHO);

        controls_ = Controls{input()};

        controls_.registerCommand(Controls::CAMERA_UP, KEY_UP);
        controls_.registerCommand(Controls::CAMERA_DOWN, KEY_DOWN);
        controls_.registerCommand(Controls::CAMERA_LEFT, KEY_LEFT);
        controls_.registerCommand(Controls::CAMERA_RIGHT, KEY_RIGHT);
        controls_.registerCommand(Controls::FORWARD, KEY_W);
        controls_.registerCommand(Controls::BACKWARD, KEY_S);
        controls_.registerCommand(Controls::LEFT, KEY_A);
        controls_.registerCommand(Controls::RIGHT, KEY_D);
        controls_.registerCommand(Controls::QUIT, KEY_ESCAPE);

        controls_.setPanSpeed(200.0f);
        controls_.setZoomSpeed(2.0f);

        auto* myEntity = new Entity{};

        if (!PB::LoadAnimationsPack(Constants::Animation::Pack::kBasicHuman))
        {
            std::cout << "Failed to load animation pack" << std::endl;
        }

        PB::LoadFontAsset(Constants::Font::kMochiyPop, 72);

        getCamera().moveTo({0.0f, 0.0f, 0.0f});

        if (PB::CreateSceneObject("Assets1/Sprites/GenericMob", myEntity, PB::LibraryAsset::Type::MODEL_2D))
        {
            myEntity->name = "Fred";
            myEntity->position = PB::vec3{150.0f, 50.0f, -50.0f};
//            myEntity->setBehavior(PB::AI::Behavior::WANDER);
            addSceneObject(myEntity);
        }

        player = myEntity;

        auto* weapon = new Entity();

        if (PB::CreateSceneObject("Assets1/Sprites/Weapons/Knife", weapon, PB::LibraryAsset::Type::MODEL_2D))
        {
            weapon->name = "weapon";
            weapon->position = {0.0f, 0.0f, 0.0f};
            addSceneObject(weapon);
            weapon->attachTo(myEntity, "weapon_attach_right");
        }

        auto* chain = new Entity();

        if (PB::CreateSceneObject("Assets1/Sprites/Misc/Chain", chain, PB::LibraryAsset::Type::MODEL_2D))
        {
            chain->name = "chain";
            chain->position = {0.0f, 0.0f, -40.0f};
            addSceneObject(chain);
        }

        bool error = false;

        {
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

            userInput_.targetComponent(inputBox);

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

            uiController_.addComponent(groupComponent, INPUT_BOX);
        }

        {
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

            uiController_.addComponent(fpsCounter, FPS_BOX);
        }
    }

protected:
    void updates(float deltaTime) override
    {
        bool error = false;

        frameRates_[frameIndex_] = deltaTime;
        timeSinceFpsCheck += deltaTime;
        frameIndex_ = (frameIndex_ + 1) % 60;

        if (timeSinceFpsCheck > 0.1)
        {
            timeSinceFpsCheck -= 0.25f;
            std::uint32_t averageFps = calculateAverageFps(frameRates_, 60);
            uiController_.getComponent(FPS_BOX, &error)->setStringAttribute(PB::UI::TEXT_CONTENT,
                                                                            std::to_string(averageFps) + " FPS");
        }

        if (!userInput_.isReading() && !userInput_.isEmpty())
        {
            std::string input = userInput_.read();
            userInput_.clear();

            std::cout << "You typed: " << input << std::endl;

            if (input == "/horizontal")
            {
                uiController_.getComponent(INPUT_BOX, &error)
                        ->setUIntAttribute(PB::UI::LAYOUT, PB::UI::Layout::HORIZONTAL);
            }
            else if (input == "/vertical")
            {
                uiController_.getComponent(INPUT_BOX, &error)
                        ->setUIntAttribute(PB::UI::LAYOUT, PB::UI::Layout::VERTICAL);
            }
            else if (input == "/ortho")
            {
                setViewMode(PB::SceneView::ORTHO);
            }
            else if (input == "/perspective" || input == "/persp")
            {
                setViewMode(PB::SceneView::PERSPECTIVE);
            }
        }
        else
        {
            userInput_.component()->setStringAttribute(PB::UI::TEXT_CONTENT, userInput_.read());
        }

        uiController_.update(deltaTime);
    }

    void renders() const override
    {
        uiController_.render();
    }

    void processInputs() override
    {
        if (input()->mouse.isReleased(BTN_LEFT))
        {
            std::cout << "Clicked at: " << input()->mouse.x << ", " << input()->mouse.y << std::endl;
        }

        if (userInput_.isReading())
        {
            if (input()->keyboard.isReleased(KEY_ENTER))
            {
                userInput_.stopReading();
            }
            else
            {
                // TODO: Still need to account for shift values
                bool shiftDown = input()->keyboard.isDown(KEY_LEFT_SHIFT) || input()->keyboard.isDown(KEY_RIGHT_SHIFT);

                // TODO: This can probably be better
                std::vector<std::uint8_t> releasedKeys = input()->keyboard
                        .areReleased(Constants::Input::inputKeys, Constants::Input::inputKeyCount);

                for (auto k : releasedKeys)
                {
                    userInput_.append(PB::GetCharFromCode(k, shiftDown));
                }

                if (input()->keyboard.isReleased(KEY_BACKSPACE))
                {
                    userInput_.removeBeforeCursor();
                }
            }
        }
        else
        {
            if (input()->keyboard.isReleased(KEY_ENTER))
            {
                userInput_.startReading();
            }
            else
            {
                if (controls_.isCommandReleased(Controls::QUIT))
                {
                    input()->window.windowClose = true;
                }

                PB::vec3 moveVec{};
                PB::vec3 cameraMoveVec{};

                bool snapCamera = false;

                if (controls_.isCommandActive(Controls::FORWARD) || controls_.isCommandActive(Controls::BACKWARD))
                {
                    moveVec.y = (
                            controls_.isCommandActive(Controls::FORWARD)
                            + (-1 * controls_.isCommandActive(Controls::BACKWARD))
                    );

                    snapCamera = true;
                }

                if (controls_.isCommandActive(Controls::RIGHT) || controls_.isCommandActive(Controls::LEFT))
                {
                    moveVec.x = (
                            controls_.isCommandActive(Controls::RIGHT)
                            + (-1 * controls_.isCommandActive(Controls::LEFT))
                    );

                    snapCamera = true;
                }

                if (controls_.isCommandActive(Controls::CAMERA_UP) || controls_.isCommandActive(Controls::CAMERA_DOWN))
                {
                    cameraMoveVec.y = (
                            controls_.isCommandActive(Controls::CAMERA_UP)
                            + (-1 * controls_.isCommandActive(Controls::CAMERA_DOWN))
                    );
                }

                if (controls_.isCommandActive(Controls::CAMERA_LEFT) || controls_.isCommandActive(Controls::CAMERA_RIGHT))
                {
                    cameraMoveVec.x = (
                            controls_.isCommandActive(Controls::CAMERA_RIGHT)
                            + (-1 * controls_.isCommandActive(Controls::CAMERA_LEFT))
                    );
                }

                if (getViewMode() == PB::SceneView::ORTHO)
                {
                    player->moveVector = moveVec;

                    getCamera().move(cameraMoveVec * controls_.getPanSpeed());

                    if (snapCamera)
                    {
                        getCamera().moveNear(
                                {player->position.x, player->position.y, 0.0f},
                                {100.0f, 100.0f, 0.0f}
                        );
                    }
                }
                else
                {
                    // +forward would be backwards since +z is toward user, so invert it
                    getCamera().directionalMove({moveVec.x, 0.0f, -moveVec.y});
                    // +yaw = turning left, so invert x value
                    getCamera().rotate(PB::vec3{cameraMoveVec.y, -cameraMoveVec.x, 0.0f});
                }
            }
        }

        if (input()->window.newWidth != 0 || input()->window.newHeight != 0)
        {
            std::cout << "Window Size: " << std::to_string(input()->window.newWidth) << "x"
                      << std::to_string(input()->window.newHeight) << std::endl;
        }

        if (input()->mouse.wheelYDir != 0)
        {
            getCamera().zoom(static_cast<std::int8_t>(input()->mouse.wheelYDir) * controls_.getZoomSpeed());
        }
    }

private:
    UIController uiController_{};
    UserInput userInput_{};
    Controls controls_{nullptr};
    float frameRates_[60];
    std::uint8_t frameIndex_ = 0;
    Entity* player = nullptr;
};