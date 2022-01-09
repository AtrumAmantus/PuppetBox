#include "PropertyTree.h"

#include <utility>

#include "Logger.h"

namespace PB
{
    PropertyTree::PropertyTree() : parent_(nullptr)
    {

    }

    PropertyTree::PropertyTree(std::string name, PropertyTree* parent) : name_(std::move(name)), parent_(parent)
    {

    }

    PropertyTree* PropertyTree::get(const std::string& parameterName)
    {
        if (children_.find(parameterName) != children_.end())
        {
            return &(*children_.at(parameterName));
        }

        return nullptr;
    }

    std::string PropertyTree::name()
    {
        return name_;
    }

    std::string PropertyTree::value()
    {
        if (children_.size() == 1 && children_.begin()->second->isLeaf())
        {
            return children_.begin()->second->name_;
        }

        LOGGER_ERROR("Node does not have a singular child leaf node, no value to retrieve");
        return "";
    }

    bool PropertyTree::has(const std::string& parameterName)
    {
        return children_.find(parameterName) != children_.end();
    }

    bool PropertyTree::add(const std::string& parameterName)
    {
        bool exists = has(parameterName);

        children_[parameterName] = std::make_shared<PropertyTree>(parameterName, this);

        return !exists;
    }

    bool PropertyTree::remove(const std::string& parameterName)
    {
        return children_.erase(parameterName) > 0;
    }

    bool PropertyTree::isLeaf()
    {
        return children_.empty();
    }

    std::vector<std::string> PropertyTree::children()
    {
        std::vector<std::string> children{};

        for (auto itr = children_.begin(); itr != children_.end(); ++itr)
        {
            children.push_back(itr->first);
        }

        return children;
    }

    PropertyTree* PropertyTree::parent()
    {
        return parent_;
    }
}