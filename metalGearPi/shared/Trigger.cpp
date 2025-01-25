#include "Trigger.h"
#include "collisionManager.h"


Trigger::Trigger(btDiscreteDynamicsWorld& p_dynamicsWorld, btCollisionShape* p_shape, const btVector3 p_pos, const std::function<void()> p_callback, CollisionManager::CollisionMask p_group, CollisionManager::CollisionMask p_mask)
    : m_shape(p_shape), m_callback(p_callback)
{
    m_ghostObject = new btGhostObject();
    m_ghostObject->setCollisionShape(m_shape);
    m_ghostObject->setCollisionFlags(m_ghostObject->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(p_pos);
    m_ghostObject->setWorldTransform(transform);
    p_dynamicsWorld.addCollisionObject(m_ghostObject, p_group, p_mask);
    
}

void Trigger::CheckOverlap()
{
    for (int i = 0; i < m_ghostObject->getNumOverlappingObjects(); ++i)
    {
        btCollisionObject* obj = m_ghostObject->getOverlappingObject(i);
        if (obj && m_callback) 
        {
            m_callback();
            return;
        }
    }
}

void Trigger::UpdatePosition(const glm::vec3& p_newPosition)
{
    btTransform transform = m_ghostObject->getWorldTransform();

    btMatrix3x3 currentBasis = transform.getBasis();
    transform.setOrigin(btVector3(p_newPosition.x, p_newPosition.y, p_newPosition.z));
    transform.setBasis(currentBasis);

    m_ghostObject->setWorldTransform(transform);
}
