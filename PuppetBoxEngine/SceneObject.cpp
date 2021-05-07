#include "pch.h"

#include "../include/puppetbox/SceneObject.h"

#include "WanderBehavior.h"

namespace PB
{
	const uint32_t SceneObject::id() const
	{
		return id_;
	}

	void SceneObject::update(float deltaTime)
	{
		updates(deltaTime);
		vec3 transform[3] = { position, rotation, actualScale() };
		if (model_ != nullptr)
		{
			model_->updateModelMatrix(transform);
		}
		if (behavior_ != nullptr)
		{
			behavior_->update(this, deltaTime);
		}
	};

	void SceneObject::render()
	{
		model_->render();
	};

	void SceneObject::setBehavior(AI::Behavior behavior)
	{
		switch (behavior)
		{
		case AI::Behavior::WANDER:
			behavior_ = std::make_unique<WanderBehavior>();
			behavior_->init(this);
			break;
		default:
			behavior_ = nullptr;
		}
	}

	void SceneObject::setBehavior(std::unique_ptr<IBehavior> behavior)
	{
		behavior_ = std::move(behavior);
	}

	vec3 SceneObject::actualScale()
	{
		return vec3{
			scale.x * baseScale_.x,
			scale.y * baseScale_.y,
			scale.z * baseScale_.z
		};
	}
}