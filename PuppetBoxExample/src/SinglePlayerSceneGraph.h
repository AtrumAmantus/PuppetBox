#pragma once

#include <iostream>
#include <string>

#include <puppetbox/AbstractSceneGraph.h>
#include <puppetbox/Constants.h>
#include <puppetbox/RenderData.h>
#include <puppetbox/Utilities.h>

#include "Constants.h"
#include "SceneObjectBuilder.h"
#include "ScreenTranslator.h"

class SinglePlayerSceneGraph : public PB::AbstractSceneGraph
{
public:
    SinglePlayerSceneGraph(const std::string& sceneName) : PB::AbstractSceneGraph(sceneName) {};

protected:
    bool setUps() override
    {
        setViewMode(PB::SceneView::ORTHO);

        bool success = true;

        if (PB::LoadAssetPack("Assets1"))
        {
            bool error = false;

            camera().moveTo({0.0f, 0.0f, 3.0f});
            camera().setPanSpeed(100.0f);
            camera().setZoomSpeed(2.0f);

            SceneObject object = SceneObjectBuilder::newObject()
                    .position({0.0f, 0.0f, -40.0f})
                    .skeleton(Constant::Asset::Skeleton::kHuman)
                    .build();

            PB::BoneMap boneMap = PB::GetSkeletonAsset(Constant::Asset::Skeleton::kHuman, &error);

            if (buildSceneObject(object))
            {
                PB::UUID headSpriteUUID = createSprite(Constant::Asset::Texture::kHumanHead, {32.0f, 32.0f, 1.0f}, &error);
                attachToSceneObject(headSpriteUUID, object.uuid, boneMap.getBoneId(Constant::Reference::Skeleton::kHead));

                PB::UUID bodySpriteUUID = createSprite(Constant::Asset::Texture::kHumanBody, {32.0f, 32.0f, 1.0f}, &error);
                attachToSceneObject(bodySpriteUUID, object.uuid, boneMap.getBoneId(Constant::Reference::Skeleton::kRoot));

                PB::UUID lhSpriteUUID = createSprite(Constant::Asset::Texture::kHumanHand, {16.0f, 16.0f, 1.0f}, &error);
                attachToSceneObject(lhSpriteUUID, object.uuid, boneMap.getBoneId(Constant::Reference::Skeleton::kLeftHand));

                PB::UUID rhSpriteUUID = createSprite(Constant::Asset::Texture::kHumanHand, {16.0f, 16.0f, 1.0f}, &error);
                attachToSceneObject(rhSpriteUUID, object.uuid, boneMap.getBoneId(Constant::Reference::Skeleton::kRightHand));
            }
            else
            {
                std::cout << "Failed to create scene object" << std::endl;
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

private:
    bool buildSceneObject(SceneObject sceneObject)
    {
        bool error = false;

        PB::BoneMap boneMap = PB::GetSkeletonAsset(sceneObject.skeleton, &error);

        if (!error)
        {
            struct ModelData {
                std::string name;
                std::uint32_t meshId;
                PB::mat4 transform;
                std::uint32_t shaderId;
                std::vector<PB::ImageMap> imageMaps{};
            };

            std::vector<ModelData> modelsToAdd{};

            for (auto& model : sceneObject.models)
            {
                ModelData modelData{};
                modelData.name = model.name;
                modelData.transform = model.transform;

                if (!error)
                {
                    modelData.meshId = PB::GetMeshAsset(model.mesh, &error);

                    if (!error)
                    {
                        modelData.shaderId = PB::GetShaderAsset(model.shader, &error);

                        if (!error)
                        {
                            for (auto imageMap: model.imageMaps)
                            {
                                if (!error)
                                {
                                    std::uint32_t imageMapId = PB::GetImageMapAsset(imageMap.name, &error);

                                    if (!error)
                                    {
                                        modelData.imageMaps.push_back(PB::ImageMap{imageMapId, imageMap.type});
                                    }
                                }
                            }
                        }
                        else
                        {
                            error = true;
                            std::cout << "Failed to load Shader";
                        }
                    }
                    else
                    {
                        error = true;
                        std::cout << "Failed to load Mesh";
                    }
                }

                modelsToAdd.push_back(modelData);
            }

            if (!error)
            {
                createSceneObject(sceneObject.uuid);
                setSceneObjectPosition(sceneObject.uuid, sceneObject.position);
                setSceneObjectSkeleton(sceneObject.uuid, std::move(boneMap));

                for (auto& model : modelsToAdd)
                {
                    addModelToSceneObject(
                            model.name,
                            sceneObject.uuid,
                            PB::Model{
                                    model.meshId,
                                    model.transform,
                                    model.shaderId,
                                    std::move(model.imageMaps)});
                }
            }
        }
        else
        {
            error = true;
            std::cout << "Failed to load Skeleton";
        }

        return !error;
    };

    PB::UUID createSprite(const std::string& texturePath, PB::vec3 scale, bool* error)
    {
        PB::UUID uuid = PB::RandomUtils::uuid();

        createSceneObject(uuid);
        setSceneObjectPosition(uuid, {0.0f, 150.0f, -40.0f});

        addModelToSceneObject(
                "root",
                uuid,
                PB::Model{
                        PB::GetMeshAsset(Constant::DefaultAsset::Mesh::kSprite, error),
                        PB::mat4{
                                scale.x, 0.0f, 0.0f, 0.0f,
                                0.0f, scale.y, 0.0f, 0.0f,
                                0.0f, 0.0f, scale.z, 0.0f,
                                0.0f, 0.0f, 0.0f, 1.0f},
                        PB::GetShaderAsset(Constant::DefaultAsset::Shader::kBasicShader, error),
                        {PB::ImageMap{PB::GetImageMapAsset(texturePath, error), PB::DIFFUSE}}
                });

        return uuid;
    };

};