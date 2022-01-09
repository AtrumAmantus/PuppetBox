#pragma once

#include <string>

#include "puppetbox/SceneObject.h"

class Entity : public PB::SceneObject
{
public:
    std::string name;
    uint8_t health = 50;
protected:
    void updates(float deltaTime) override
    {

    };
};