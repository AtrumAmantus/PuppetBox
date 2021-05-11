#pragma once

#include <string>

#include "AbstractMessage.h"

namespace PB
{
	class MessageBroker
	{
	public:
		template <class T>
		T sendAndReceive(AbstractMessage* message)
		{
			return (T)sendAndReceive(message);
		};
		void subscribe(std::string topic);
	private:
		void* sendAndReceive(AbstractMessage* message);
	};
}