#pragma once

#include <iostream>
#include <string>

#include <puppetbox/AbstractSceneGraph.h>
#include <puppetbox/Constants.h>
#include <puppetbox/Utilities.h>

#include "ScreenTranslator.h"

class SingleplayerSceneGraph : public PB::AbstractSceneGraph
{
public:
    SingleplayerSceneGraph(const std::string& sceneName) : PB::AbstractSceneGraph(sceneName) {};

protected:
    bool setUps() override
    {
        setViewMode(PB::SceneView::ORTHO);

        bool success = true;

        if (PB::LoadAssetPack("Assets1"))
        {
            camera().moveTo({0.0f, 0.0f, 3.0f});
            camera().setPanSpeed(100.0f);
            camera().setZoomSpeed(2.0f);

            PB::UUID uuid = PB::RandomUtils::uuid();
            createSceneObject(uuid);
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

        return success;
    }

    void preLoopUpdates(float deltaTime) override
    {

    }

    void postLoopUpdates(float deltaTime) override
    {

    }

    void renders() const override
    {

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


    }

private:
    ScreenTranslator screenTranslator_{};
};