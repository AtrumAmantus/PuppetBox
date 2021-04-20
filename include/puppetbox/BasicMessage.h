#pragma once

#include "../../PuppetBoxEngine/IMessage.h"

namespace PB
{
	class BasicMessage : public IMessage
	{
	public:
		BasicMessage(const Event::Type type, const std::string message) : type_(type), message_(message) {};
		const Event::Type getType() const
		{
			return type_;
		};
		const std::string getString() const
		{
			return message_;
		};
	private:
		const Event::Type type_;
		const std::string message_;
	};
}