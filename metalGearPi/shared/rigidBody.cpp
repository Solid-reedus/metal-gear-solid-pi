#include "rigidBody.h"

#if defined(LINUX_BUILD) || defined(WINDOWS_BUILD)
#include <bullet/btBulletDynamicsCommon.h>
#endif // WINDOWS_BUILD

#include "collisionManager.h"

RigidBody::RigidBody()
{
	m_shape = nullptr;
	motionState = nullptr;
	m_body = nullptr;
}


RigidBody::RigidBody(btDiscreteDynamicsWorld& p_dynamicsWorld, btCollisionShape* p_shape, btVector3 p_pos, float p_mass, int p_group, int p_mask)
{
	m_shape = p_shape;

	btTransform groundTransform;
	groundTransform.setIdentity();
	groundTransform.setOrigin(p_pos);

	btVector3 localInertia(0, 0, 0);
	if (p_mass != 0.f)
	{
		m_shape->calculateLocalInertia(p_mass, localInertia);
	}

	motionState = new btDefaultMotionState(groundTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(p_mass, motionState, m_shape, localInertia);
	m_body = new btRigidBody(rbInfo);

	//add the body to the dynamics world
	p_dynamicsWorld.addRigidBody(m_body, p_group, p_mask);
}

