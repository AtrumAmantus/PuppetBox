#include "PropertyTree.h"

#include "Logger.h"

namespace PB
{
	PropertyTree::PropertyTree() : value_(""), parent_(nullptr)
	{

	}

	PropertyTree::PropertyTree(std::string value, PropertyTree* parent) : value_(value), parent_(parent)
	{

	}

	PropertyTree* PropertyTree::get(std::string parameterName)
	{
		if (children_.find(parameterName) != children_.end())
		{
			return &(*children_.at(parameterName));
		}

		return nullptr;
	}

	std::string PropertyTree::value()
	{
		if (children_.size() == 1 && children_.begin()->second->isLeaf())
		{
			return children_.begin()->second->value_;
		}
		
		LOGGER_ERROR("Node does not have a singular child leaf node, no value to retrieve");
		return "";
	}

	bool PropertyTree::has(std::string parameterName)
	{
		return children_.find(parameterName) != children_.end();
	}

	bool PropertyTree::add(std::string parameterName)
	{
		bool exists = has(parameterName);

		children_.insert(
			std::pair<std::string, PropertyTree*>{ parameterName, new PropertyTree(parameterName, this) }
		);

		return !exists;
	}

	bool PropertyTree::remove(std::string parameterName)
	{
		return children_.erase(parameterName) > 0;
	}

	bool PropertyTree::isLeaf()
	{
		return children_.empty();
	}

	PropertyTree* PropertyTree::parent()
	{
		return parent_;
	}
}