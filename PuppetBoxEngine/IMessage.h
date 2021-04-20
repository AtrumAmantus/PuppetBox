#pragma once

#include <string>

namespace PB
{
	namespace Event
	{
		enum Type
		{
			UNKNOWN,
			ADD_TO_SCENE,
		};
	}

	class IMessage
	{
	public:
		virtual const Event::Type getType() const = 0;
		virtual const std::string getString() const = 0;
	};
}