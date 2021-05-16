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
		explicit PropertyTree(std::string value, PropertyTree* parent = nullptr);
		PropertyTree* get(const std::string& parameterName);
		std::string value();
		bool has(const std::string& parameterName);
		bool add(const std::string& parameterName);
		bool remove(const std::string& parameterName);
		bool isLeaf();
		PropertyTree* parent();
	private:
		std::string value_;
		std::unordered_map<std::string, std::shared_ptr<PropertyTree>> children_{};
		PropertyTree* parent_;
	};
}