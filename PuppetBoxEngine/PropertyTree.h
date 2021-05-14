#pragma once

#include <memory>
#include <string>
#include <unordered_map>

namespace PB
{
	class PropertyTree
	{
	public:
		PropertyTree();
		PropertyTree(std::string value, PropertyTree* parent = nullptr);
		PropertyTree* get(std::string parameterName);
		std::string value();
		bool has(std::string parameterName);
		bool add(std::string parameterName);
		bool remove(std::string parameterName);
		bool isLeaf();
		PropertyTree* parent();
	private:
		std::string value_;
		std::unordered_map<std::string, std::shared_ptr<PropertyTree>> children_{};
		PropertyTree* parent_;
	};
}