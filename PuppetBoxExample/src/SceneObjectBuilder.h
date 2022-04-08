#pragma once

#include <vector>

#include <puppetbox/DataStructures.h>
#include <puppetbox/Utilities.h>

struct SceneObjectImageMap
{
    const std::string name;
    const PB::ImageMapType type;
};

struct SceneObjectModel
{
    const std::string name;
    const std::string mesh;
    const PB::mat4 transform;
    const std::string shader;
    const std::vector<SceneObjectImageMap> imageMaps;
};

struct SceneObject
{
    const PB::UUID uuid;
    const PB::vec3 position;
    const std::string skeleton;
    const std::vector<SceneObjectModel> models;
};

class SceneObjectModelBuilder
{
public:
    static SceneObjectModelBuilder newModel(const std::string& name)
    {
        auto object = SceneObjectModelBuilder{};
        object.name_ = name;

        return object;
    };

    SceneObjectModelBuilder& mesh(const std::string& mesh)
    {
        mesh_ = mesh;
        return *this;
    };

    SceneObjectModelBuilder& transform(PB::mat4 transform)
    {
        transform_ = transform;
        return *this;
    };

    SceneObjectModelBuilder& shader(const std::string& shader)
    {
        shader_ = shader;
        return *this;
    };

    SceneObjectModelBuilder& addImageMap(const std::string& imageMap, PB::ImageMapType mapType)
    {
        imageMaps_.push_back({imageMap, mapType});
        return *this;
    };

    SceneObjectModel build()
    {
        return SceneObjectModel{
            name_,
            mesh_,
            transform_,
            shader_,
            imageMaps_
        };
    };

private:
    std::string name_;
    std::string mesh_;
    PB::mat4 transform_ = PB::mat4::eye();
    std::string shader_;
    std::vector<SceneObjectImageMap> imageMaps_{};

};

class SceneObjectBuilder
{
public:
    static SceneObjectBuilder newObject(PB::UUID uuid)
    {
        SceneObjectBuilder o = SceneObjectBuilder{};
        o.uuid_ = uuid;

        return o;
    };

    static SceneObjectBuilder newObject()
    {
        return newObject(PB::RandomUtils::uuid());
    };

    SceneObjectBuilder& position(PB::vec3 position)
    {
        position_ = position;
        return *this;
    };

    SceneObjectBuilder& skeleton(const std::string& skeleton)
    {
        skeleton_ = skeleton;
        return *this;
    };

    SceneObjectBuilder& addModel(SceneObjectModel model)
    {
        models_.push_back(model);
        return *this;
    };

    SceneObject build()
    {
        return SceneObject {
            uuid_,
            position_,
            skeleton_,
            models_
        };
    };

private:
    PB::UUID uuid_{};
    PB::vec3 position_{};
    std::string skeleton_;
    std::vector<SceneObjectModel> models_{};

};