#pragma once

#include <iostream>
#include <memory>
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
#include "Sprite.h"
#include "UIAttributeBuilder.h"
#include "UserInput.h"

class CustomSceneHandler : public PB::AbstractSceneHandler
{
public:
    void setUp() override
    {
        controls_ = Controls{input()};

        controls_.registerCommand(Controls::FORWARD, KEY_UP);
        controls_.registerCommand(Controls::FORWARD, KEY_W);
        controls_.registerCommand(Controls::BACKWARD, KEY_DOWN);
        controls_.registerCommand(Controls::BACKWARD, KEY_S);
        controls_.registerCommand(Controls::LEFT, KEY_LEFT);
        controls_.registerCommand(Controls::LEFT, KEY_A);
        controls_.registerCommand(Controls::RIGHT, KEY_RIGHT);
        controls_.registerCommand(Controls::RIGHT, KEY_D);
        controls_.registerCommand(Controls::QUIT, KEY_ESCAPE);

        controls_.setMoveSpeed(200.0f);
        controls_.setZoomSpeed(2.0f);

        auto* myEntity = new Entity{};

        PB::IAnimationCatalogue* anims = PB::CreateAnimationCatalogue();

        PB::LoadFontAsset("Assets1/Fonts/MochiyPop/Regular", 72);

        getCamera()->setPosition({-400, -300, 0});

        if (PB::CreateSceneObject("Assets1/Sprites/GenericMob", myEntity, PB::LibraryAsset::Type::MODEL_2D))
        {
            myEntity->name = "Fred";
            myEntity->position = PB::vec3{100.0f, 50.0f, 50.0f};
            myEntity->setBehavior(PB::AI::Behavior::WANDER);
            addSceneObject(myEntity);
        }

        if (anims->load("Assets1/Animations/BasicHuman"))
        {
            myEntity->playAnimation(anims->get("walk"));
        }

        {
            auto builder = UIAttributeBuilder{}
                    .origin(PB::UI::Origin::TOP_LEFT)
                    .position({0, 400, 1})
                    .dimensions({300, 200})
                    .fontSize(24)
                    .fontType(Constants::Font::MochiyPop);

            bool error = false;

            auto textBox = std::shared_ptr<PB::UIComponent>(
                    PB::CreateUIComponent(
                            PB::UI::TEXT_AREA,
                            std::move(builder.build()),
                            &error
                    )
            );

            textBox->setStringAttribute(PB::UI::TEXT_CONTENT,
                                        "This is a long sentence that should wrap to the next line, followed by some additional words to check to see how big the bounding box is, and if it is properly clipping the text.");

            auto inputBox = std::shared_ptr<PB::UIComponent>(
                    PB::CreateUIComponent(
                            PB::UI::TEXT_AREA,
                            std::move(builder.build()),
                            &error
                    )
            );

            inputBox->setUIntAttribute(PB::UI::ORIGIN, PB::UI::Origin::BOTTOM_LEFT);
            inputBox->setUIntAttribute(PB::UI::POS_Y, 200 - 24);
            inputBox->setUIntAttribute(PB::UI::HEIGHT, 24);

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

            addToUI(groupComponent);
        }
    }

protected:
    void updates(float deltaTime) override
    {
        if (!userInput_.isReading() && !userInput_.isEmpty())
        {
            std::string input = userInput_.read();
            userInput_.clear();

            std::cout << "You typed: " << input << std::endl;

            if (input == "/horizontal")
            {
                uiComponents_.at(0)->setUIntAttribute(PB::UI::LAYOUT, PB::UI::Layout::HORIZONTAL);
            }
            else if (input == "/vertical")
            {
                uiComponents_.at(0)->setUIntAttribute(PB::UI::LAYOUT, PB::UI::Layout::VERTICAL);
            }
        }
        else
        {
            userInput_.component()->setStringAttribute(PB::UI::TEXT_CONTENT, userInput_.read());
        }

        for (auto& component: uiComponents_)
        {
            component->update(deltaTime);
        }
    }

    void renders() const override
    {
        for (auto& component: uiComponents_)
        {
            component->render();
        }
    }

    void processInputs() override
    {
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
                    userInput_.append(PB::GetCharFromCode(k));
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

                if (controls_.isCommandActive(Controls::FORWARD) || controls_.isCommandActive(Controls::BACKWARD))
                {
                    moveVec.y = (
                                        controls_.isCommandActive(Controls::FORWARD)
                                        + (-1 * controls_.isCommandActive(Controls::BACKWARD))
                                ) * controls_.getMoveSpeed();
                }

                if (controls_.isCommandActive(Controls::RIGHT) || controls_.isCommandActive(Controls::LEFT))
                {
                    moveVec.x = (
                                        controls_.isCommandActive(Controls::RIGHT)
                                        + (-1 * controls_.isCommandActive(Controls::LEFT))
                                ) * controls_.getMoveSpeed();
                }

                getCamera()->move(moveVec);
            }
        }

        if (input()->window.newWidth != 0 || input()->window.newHeight != 0)
        {
            std::cout << "Window Size: " << std::to_string(input()->window.newWidth) << "x"
                      << std::to_string(input()->window.newHeight) << std::endl;
        }

        if (input()->mouse.wheelYDir != 0)
        {
            getCamera()->zoom(static_cast<std::int8_t>(input()->mouse.wheelYDir) * controls_.getZoomSpeed());
        }
    }

private:
    std::vector<std::shared_ptr<PB::UIComponent>> uiComponents_{};
    UserInput userInput_{};
    Controls controls_{nullptr};
private:
    void addToUI(std::shared_ptr<PB::UIComponent> component)
    {
        uiComponents_.push_back(std::move(component));
    }
};