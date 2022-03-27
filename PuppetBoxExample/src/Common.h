#pragma once

#include <iostream>

#include "EventDef.h"
#include "GameEvent.h"
#include "TypeDef.h"
#include "UIAttributeBuilder.h"
#include "UserInput.h"

#define FPS_MAX_FRAME_COUNT 60

float _timeSinceGfxFpsCheck = 0.0f;
std::uint32_t _frameIndex = 0;
float _frameRates[FPS_MAX_FRAME_COUNT]{};

extern inline void registerControls(InputActions& controls)
{
    controls.registerCommand(Command::CAMERA_UP, KEY_UP);
    controls.registerCommand(Command::CAMERA_DOWN, KEY_DOWN);
    controls.registerCommand(Command::CAMERA_LEFT, KEY_LEFT);
    controls.registerCommand(Command::CAMERA_RIGHT, KEY_RIGHT);
    controls.registerCommand(Command::FORWARD, KEY_W);
    controls.registerCommand(Command::BACKWARD, KEY_S);
    controls.registerCommand(Command::LEFT, KEY_A);
    controls.registerCommand(Command::RIGHT, KEY_D);
    controls.registerCommand(Command::QUIT, KEY_ESCAPE);
    controls.registerCommand(Command::EQUIP_SLOT_1, KEY_1);
    controls.registerCommand(Command::EQUIP_SLOT_2, KEY_2);
    controls.registerCommand(Command::EQUIP_SLOT_3, KEY_3);
    controls.registerCommand(Command::EQUIP_SLOT_4, KEY_4);
    controls.registerCommand(Command::EQUIP_SLOT_5, KEY_5);
}

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

extern inline bool updateFrameCounter(float deltaTime)
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

// TODO: Make a DSL for this
extern inline bool uiSetup(UIController& uiController, UserInput& userInput)
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
                                "This is a long sentence that should wrap to the next line, followed by some "
                                "additional words to check to see how big the bounding box is, and if it is properly "
                                "clipping the text.\n"
                                "Another sentence that is not as long as the last one but still long enough to wrap.\n"
                                "Plus one more small one.\n");

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

    uiController.addComponent(groupComponent, CHAT_BOX);
    uiController.referenceComponent(textBox, CHAT_MESSAGES_BOX);
    uiController.referenceComponent(inputBox, CHAT_INPUT_BOX);

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