#pragma once

#include <string>

class IHardwareInitializer
{
public:
	virtual void init(std::string windowTitle, uint32_t windowWidth, uint32_t windowHeight) = 0;
	virtual bool hadError() const = 0;
};