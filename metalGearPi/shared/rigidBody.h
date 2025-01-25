#pragma once

#if defined(LINUX_BUILD) || defined(WINDOWS_BUILD)
#include <bullet/btBulletDynamicsCommon.h>
#endif // WINDOWS_BUILD

#include "collisionManager.h"


class RigidBody
{
	public:
	RigidBody(); // default contructor
	RigidBody(btDiscreteDynamicsWorld& p_dynamicsWorld, btCollisionShape* p_shape, btVector3 p_pos, float p_mass, int p_group, int p_mask);
	btRigidBody* m_body = nullptr;

	private:
	btCollisionShape* m_shape = nullptr;
	btDefaultMotionState* motionState = nullptr;

};

