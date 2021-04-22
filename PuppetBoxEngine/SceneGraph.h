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
			// Nothing yet
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
	};
}