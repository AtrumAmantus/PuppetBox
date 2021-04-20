#pragma once

#include <functional>

#include "IMessage.h"

namespace PB
{
	typedef void* (*ProcAddress)(const char* name);

	using EventListener = std::function<void(IMessage*)>;
}