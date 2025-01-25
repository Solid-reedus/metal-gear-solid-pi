#pragma once

#ifdef LINUX_BUILD
#include <bullet/btBulletDynamicsCommon.h>
#include <glm/glm.hpp>
#endif // LINUX_BUILD

#ifdef WINDOWS_BUILD
#include <bullet/btBulletDynamicsCommon.h>
#include "glm.hpp"
#endif // WINDOWS_BUILD

//#include "baseShader.h"
#include <stdio.h>
#include <functional>
#include <vector>

//class Shader;
class TriggerManager;
class DebugDrawer;
class BaseShader;
struct Camera;
class Mesh;

class CollisionManager
{
	public:

	enum CollisionMask
	{
		NoCollision = 0 << 0,
		Player = 1 << 0,
		Enemy = 1 << 1,
		Environment = 1 << 2,
		Prop = 1 << 3,
		Trigger = 1 << 4,
		All = ~0
	};


	CollisionManager();
	void Update(float p_deltaTime);
	void Render(const Camera& p_cam);
	void ClearWorldObjects();
	void ClearSpecificObjects(CollisionManager::CollisionMask mask);
	void AddTrigger(int p_shapeIndex, const btVector3 p_pos, const std::function<void()> p_callback, CollisionMask p_group, CollisionMask p_mask);
	btCollisionShape* CreateMeshCollider(const Mesh& p_mesh, bool p_isConvex, const glm::vec3& scale = glm::vec3(1.0f));
	btCollisionShape* CreateInvertedMeshCollider(const Mesh& p_mesh, const glm::vec3& scale = glm::vec3(1.0f));
	void AddShape(btCollisionShape* p_newShape);

	~CollisionManager();

	btCollisionShape* GetShape(int p_index) { return m_collisionShapes[p_index]; };
	btDiscreteDynamicsWorld* m_dynamicsWorld;
	TriggerManager* m_triggerManager = nullptr;

	private:

	void InitShapes();

	btCollisionDispatcher* m_dispatcher;
	btDefaultCollisionConfiguration* m_collisionConfiguration;
	btBroadphaseInterface* m_overlappingPairCache;
	btSequentialImpulseConstraintSolver* m_solver;
	btAlignedObjectArray<btCollisionShape*> m_collisionShapes;
	std::vector<btCollisionShape*> m_initializedShapes; // this vector is used to track shapes that need to be deleted differently

	BaseShader* m_lineShader = nullptr;
	DebugDrawer* m_debugDrawer = nullptr;
};

