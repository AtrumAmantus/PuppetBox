#pragma once

#include <iostream>
#include <string>

#include <puppetbox/AbstractSceneGraph.h>
#include <puppetbox/Constants.h>
#include <puppetbox/RenderData.h>
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

            PB::UUID entityUUID = PB::RandomUtils::uuid();
            createSceneObject(entityUUID);
            setSceneObjectPosition(entityUUID, {0.0f, 0.0f, -40.0f});

            bool error = false;
            std::uint32_t meshId = PB::GetMeshAsset("Default/Mesh/Sprite", &error);

            if (!error)
            {
                std::uint32_t imageMapId = PB::GetImageMapAsset("Assets1/Textures/TestHead", &error);

                if (!error)
                {
                    std::uint32_t shaderId = PB::GetShaderAsset("Assets1/Shaders/Basic", &error);

                    if (!error)
                    {
                        PB::BoneMap boneMap = PB::GetSkeletonAsset("Assets1/Skeletons/BasicHuman", &error);

                        if (!error)
                        {
                            std::vector<PB::ImageMap> imageMaps{};
                            imageMaps.push_back(PB::ImageMap{imageMapId, PB::DIFFUSE});

                            setSceneObjectSkeleton(entityUUID, std::move(boneMap));

                            addModelToSceneObject(
                                    "Face",
                                    entityUUID,
                                    PB::Model{
                                            meshId,
                                            PB::mat4{
                                                    32.0f, 0.0f, 0.0f, 0.0f,
                                                    0.0f, 32.0f, 0.0f, 0.0f,
                                                    0.0f, 0.0f, 1.0f, 0.0f,
                                                    0.0f, 0.0f, 0.0f, 1.0f},
                                            shaderId,
                                            std::move(imageMaps)});

                            PB::UUID instanceSetUUID = PB::RandomUtils::uuid();

                            createInstanceSet(instanceSetUUID);

                            PB::UUID instanceUUID = PB::RandomUtils::uuid();

                            addInstanceToSet(instanceUUID, instanceSetUUID);
                            attachInstanceTo(instanceUUID, instanceSetUUID, entityUUID, boneMap.getBoneId("right_hand"));
                        }
                        else
                        {
                            std::cout << "Failed to load Skeleton";
                        }
                    }
                    else
                    {
                        std::cout << "Failed to load Shader";
                    }
                }
                else
                {
                    std::cout << "Failed to load ImageMap";
                }
            }
            else
            {
                std::cout << "Failed to load Mesh";
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