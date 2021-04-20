#pragma once

#include <memory>
#include <string>
#include <vector>

#include "../include/puppetbox/AbstractSceneHandler.h"
#include "IMessage.h"
#include "Logger.h"
#include "MessageBroker.h"
#include "TypeDef.h"

namespace PB
{
	class SceneGraph
	{
	public:
		SceneGraph(std::string sceneName, MessageBroker& messageBroker) : id_(sceneName), messageBroker_(messageBroker) {};
		void init()
		{
			subscribe(Event::Type::ADD_TO_SCENE);
		};
		void setSceneHandler(AbstractSceneHandler* sceneHandler)
		{
			sceneHandler_ = sceneHandler;
			sceneHandler_->setMessageBroker(&messageBroker_);
			sceneHandler_->setUp();
		};
		void update(const float deltaTime)
		{
			sceneHandler_->update(deltaTime);
		}
		void render() const
		{
			// Nothing yet
		}
	private:
		std::string id_;
		AbstractSceneHandler* sceneHandler_ = nullptr;
		MessageBroker& messageBroker_;
	private:
		void subscribe(Event::Type type)
		{
			EventListener eventListener = [this](IMessage* message)
			{
				this->eventListener(message);
			};

			messageBroker_.subscribe(type, eventListener);
		};
		void eventListener(IMessage* message)
		{
			switch (message->getType())
			{
			case Event::Type::ADD_TO_SCENE:
				std::cout << "Event to add item to scene: '" << message->getString() << "'" << std::endl;
				break;
			default:
				LOGGER_ERROR("Scene graph did not properly handle event type " + std::to_string(message->getType()));
			}
		};
	};
}