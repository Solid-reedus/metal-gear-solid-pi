#pragma once

#include <vector>
#include "Trigger.h"

class btGhostPairCallback;

class TriggerManager
{
	public:
	TriggerManager(btDiscreteDynamicsWorld& p_dynamicsWorld);
	~TriggerManager();

	void AddTrigger(btCollisionShape* p_shape, const btVector3 p_pos, const std::function<void()> p_callback, CollisionManager::CollisionMask p_group, CollisionManager::CollisionMask p_mask)
	{
		m_triggers.push_back(Trigger(m_dynamicsWorld, p_shape, p_pos, p_callback, p_group, p_mask));
	}

	void Update();
	std::vector<Trigger> m_triggers;

	private:
	btDiscreteDynamicsWorld& m_dynamicsWorld;
	btGhostPairCallback* m_ghostPairCallback = nullptr;
};

