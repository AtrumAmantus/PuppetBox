#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "puppetbox/DataStructures.h"

namespace PB
{
    //TODO: Fix the way node values are stored/retrieved
    class PropertyTree
    {
    public:
        PropertyTree();

        explicit PropertyTree(std::string name, PropertyTree* parent = nullptr);

        Result<PropertyTree*> get(const std::string& parameterName);

        std::string name();

        std::string value();

        bool has(const std::string& parameterName);

        bool add(const std::string& parameterName);

        bool remove(const std::string& parameterName);

        bool isLeaf();

        std::vector<std::string> children();

        PropertyTree* parent();

    private:
        std::string name_;
        std::unordered_map<std::string, std::shared_ptr<PropertyTree>> children_{};
        PropertyTree* parent_;
    };
}