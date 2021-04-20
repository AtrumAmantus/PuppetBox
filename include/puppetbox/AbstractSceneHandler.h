#pragma once

#include <vector>

#include "../PuppetBox.h"
#include "../../PuppetBoxEngine/IMessage.h"
#include "../../PuppetBoxEngine/MessageBroker.h"

namespace PB
{
	extern PUPPET_BOX_API class AbstractSceneHandler
	{
	public:
		virtual void setUp() = 0;
		virtual void update(const float deltaTime) = 0;
		void setMessageBroker(MessageBroker* messageBroker)
		{
			messageBroker_ = messageBroker;
		};
		void listener(IMessage* message)
		{
			eventListener(message);
		};
		virtual void eventListener(IMessage* message) { };
	protected:
		void publishEvent(IMessage* message)
		{
			messageBroker_->publish(message);
		};
		void subscribe(Event::Type eventType)
		{
			EventListener eventListener = [this](IMessage* message)
			{
				this->listener(message);
			};
		};
	private:
		MessageBroker* messageBroker_;
	};
}