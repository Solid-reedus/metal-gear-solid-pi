#pragma once

#if defined(LINUX_BUILD) || defined(WINDOWS_BUILD)
#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/BulletCollision/CollisionDispatch/btGhostObject.h>
#endif // WINDOWS_BUILD

#include <functional>
#include "collisionManager.h"


class Trigger
{
	public:
	Trigger(btDiscreteDynamicsWorld& p_dynamicsWorld, btCollisionShape* p_shape, const btVector3 p_pos, const std::function<void()> p_callback, CollisionManager::CollisionMask p_group, CollisionManager::CollisionMask p_mask);
	~Trigger() {};
	void CheckOverlap();

	void UpdatePosition(const glm::vec3& p_newPosition);

	private:
	btGhostObject* m_ghostObject = nullptr;
	btCollisionShape* m_shape = nullptr;
	std::function<void()> m_callback;

};

