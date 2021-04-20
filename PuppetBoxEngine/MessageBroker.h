#pragma once

#include <queue>
#include <unordered_map>

#include "IMessage.h"
#include "TypeDef.h"

namespace PB
{
	class MessageBroker
	{
	public:
		void subscribe(Event::Type type, EventListener listener)
		{
			if (subscribers_.find(type) == subscribers_.end())
			{
				subscribers_.insert(
					std::pair<Event::Type, std::vector<EventListener>>{type, std::vector<EventListener>{}}
				);
			}

			subscribers_.at(type).push_back(listener);
		};

		void publish(IMessage* message)
		{
			if (subscribers_.find(message->getType()) != subscribers_.end())
			{
				for (auto f : subscribers_.at(message->getType()))
				{
					f(message);
				}
			}
		};
	private:
		std::unordered_map<Event::Type, std::vector<EventListener>> subscribers_{};
	};
}