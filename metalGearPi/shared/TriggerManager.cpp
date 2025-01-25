#include "TriggerManager.h"

#ifdef LINUX_BUILD
#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/BulletCollision/BroadphaseCollision/btOverlappingPairCache.h>
#include <glm/glm.hpp>
#endif // LINUX_BUILD

#ifdef WINDOWS_BUILD
#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/BulletCollision/BroadphaseCollision/btOverlappingPairCache.h>
#include "glm.hpp"
#endif // WINDOWS_BUILD

TriggerManager::TriggerManager(btDiscreteDynamicsWorld& p_dynamicsWorld) : m_dynamicsWorld(p_dynamicsWorld)
{
    m_ghostPairCallback = new btGhostPairCallback();
    p_dynamicsWorld.getBroadphase()->getOverlappingPairCache()->setInternalGhostPairCallback(m_ghostPairCallback);
}

TriggerManager::~TriggerManager()
{
    delete m_ghostPairCallback;
}


void TriggerManager::Update()
{
	for (auto& trigger : m_triggers)
	{
		trigger.CheckOverlap();
	}
}
