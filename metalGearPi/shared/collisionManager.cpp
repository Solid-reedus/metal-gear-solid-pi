#include "collisionManager.h"


#ifdef LINUX_BUILD
#include <bullet/btBulletDynamicsCommon.h>
#include "../gameLinux/lnx_shader.h"
#include <glm/glm.hpp>
#endif // LINUX_BUILD

#ifdef WINDOWS_BUILD
#include <bullet/btBulletDynamicsCommon.h>
#include "../gameWindows/win_shader.h"
#include "glm.hpp"
#endif // WINDOWS_BUILD

#include "TriggerManager.h"
#include <unordered_set>
#include "debugDrawer.h"
#include "baseShader.h"
#include "camera.h"
#include <stdio.h>
#include <vector>
#include "mesh.h"

CollisionManager::CollisionManager()
{
    m_collisionConfiguration = new btDefaultCollisionConfiguration();
    m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);
    m_overlappingPairCache = new btDbvtBroadphase();
    m_solver = new btSequentialImpulseConstraintSolver;

    InitShapes();
    m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_overlappingPairCache, m_solver, m_collisionConfiguration);
    m_dynamicsWorld->setGravity(btVector3(0, -9.81f, 0));


    #ifdef LINUX_BUILD
    m_lineShader = new LnxShader("../gameLinux/debug_vert_lnx.glsl", "../gameLinux/debug_frag_lnx.glsl"); // Basic shader for lines
    #endif // LINUX_BUILD


    #ifdef WINDOWS_BUILD
    m_lineShader = new WinShader("../debug_vert_win.glsl", "../debug_frag_win.glsl"); // Basic shader for lines
    #endif // WINDOWS_BUILD


    m_debugDrawer = new DebugDrawer(m_lineShader);

    m_dynamicsWorld->setDebugDrawer(m_debugDrawer);
    m_debugDrawer->setDebugMode(btIDebugDraw::DBG_DrawWireframe);

    // Enable specific debug modes (e.g., draw wireframes)
    m_debugDrawer->setDebugMode(btIDebugDraw::DBG_DrawWireframe);

    m_triggerManager = new TriggerManager(*m_dynamicsWorld);

}

void CollisionManager::InitShapes()
{
    m_initializedShapes.push_back(new btBoxShape(btVector3(1.0f, 1.0f, 1.0f)));
    m_initializedShapes.push_back(new btBoxShape(btVector3(200.0f, 0.1f, 200.0f)));
    m_initializedShapes.push_back(new btCapsuleShape(1.0f, 5.0f));
    m_initializedShapes.push_back(new btBoxShape(btVector3(2.0f, 2.0f, 1.0f)));
    m_initializedShapes.push_back(new btBoxShape(btVector3(3.7f, 3.7f, 3.7f)));

    for (auto& shape : m_initializedShapes)
    {
        m_collisionShapes.push_back(shape);
    }

}

void CollisionManager::Update(float p_deltaTime)
{
    m_triggerManager->Update();
    m_dynamicsWorld->stepSimulation(p_deltaTime, 10, 1.0f / 60.0f);
}

void CollisionManager::Render(const Camera& p_cam)
{
    m_lineShader->use();
    m_lineShader->setMat4("projection", p_cam.GetProjectionMatrix());
    m_lineShader->setMat4("view", p_cam.GetViewMatrix());

    // Render Bullet Physics debug wireframes
    if (m_dynamicsWorld)
    {
        // Call the debug draw method
        m_dynamicsWorld->debugDrawWorld();

        // Flush lines for rendering (if necessary for your DebugDrawer implementation)
        DebugDrawer* debugDrawer = static_cast<DebugDrawer*>(m_dynamicsWorld->getDebugDrawer());
        if (debugDrawer)
        {
            debugDrawer->flushLines();
        }
    }
}

void CollisionManager::ClearWorldObjects()
{
    for (int i = m_dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
    {
        btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[i];
        btRigidBody* body = btRigidBody::upcast(obj);
        if (body && body->getMotionState())
        {
            delete body->getMotionState();
        }
        m_dynamicsWorld->removeCollisionObject(obj);
        delete obj;
    }
}

void CollisionManager::ClearSpecificObjects(CollisionManager::CollisionMask mask)
{
    for (int i = m_dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
    {
        btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[i];

        // Get the broadphase proxy handle and then access the collision group
        btBroadphaseProxy* proxy = obj->getBroadphaseHandle();
        int collisionGroup = proxy->m_collisionFilterGroup;

        // If the object belongs to the specified group, clear it
        if ((collisionGroup & mask) == mask)  // Check if the object belongs to the specified group (mask)
        {
            btRigidBody* body = btRigidBody::upcast(obj);
            if (body && body->getMotionState())
            {
                delete body->getMotionState();
            }
            m_dynamicsWorld->removeCollisionObject(obj);
            delete obj;
        }
    }
}

void CollisionManager::AddTrigger(int p_shapeIndex, const btVector3 p_pos, const std::function<void()> p_callback, CollisionMask p_group, CollisionMask p_mask)
{
    m_triggerManager->AddTrigger(GetShape(p_shapeIndex), p_pos, p_callback, p_group, p_mask);
}

btCollisionShape* CollisionManager::CreateMeshCollider(const Mesh& p_mesh, bool p_isConvex, const glm::vec3& scale)
{
    if (p_isConvex)
    {
        btConvexHullShape* convexShape = new btConvexHullShape();
        for (const auto& vertex : p_mesh.m_vertices)
        {
            glm::vec3 scaledPosition = vertex.Position * scale;
            convexShape->addPoint(btVector3(scaledPosition.x, scaledPosition.y, scaledPosition.z));
        }

        // Store the convex shape itself in the user pointer (no auxiliary resource for convex shapes)
        convexShape->setUserPointer(nullptr);
        m_collisionShapes.push_back(convexShape);
        return convexShape;
    }

    btTriangleMesh* triangleMesh = new btTriangleMesh();
    for (size_t i = 0; i < p_mesh.m_indices.size(); i += 3)
    {
        unsigned int index0 = p_mesh.m_indices[i];
        unsigned int index1 = p_mesh.m_indices[i + 1];
        unsigned int index2 = p_mesh.m_indices[i + 2];

        const auto& vertex0 = p_mesh.m_vertices[index0].Position * scale;
        const auto& vertex1 = p_mesh.m_vertices[index1].Position * scale;
        const auto& vertex2 = p_mesh.m_vertices[index2].Position * scale;

        triangleMesh->addTriangle(
            btVector3(vertex0.x, vertex0.y, vertex0.z),
            btVector3(vertex1.x, vertex1.y, vertex1.z),
            btVector3(vertex2.x, vertex2.y, vertex2.z)
        );
    }

    btBvhTriangleMeshShape* triangleMeshShape = new btBvhTriangleMeshShape(triangleMesh, true);

    // Store the triangle mesh in the user pointer
    triangleMeshShape->setUserPointer(triangleMesh);
    m_collisionShapes.push_back(triangleMeshShape);
    return triangleMeshShape;
}

btCollisionShape* CollisionManager::CreateInvertedMeshCollider(const Mesh& p_mesh, const glm::vec3& scale)
{
    // Non-convex mesh inversion
    btTriangleMesh* invertedTriangleMesh = new btTriangleMesh();
    for (size_t i = 0; i < p_mesh.m_indices.size(); i += 3)
    {
        // Get the indices for the current triangle
        unsigned int index0 = p_mesh.m_indices[i];
        unsigned int index1 = p_mesh.m_indices[i + 1];
        unsigned int index2 = p_mesh.m_indices[i + 2];

        // Access vertices and scale their positions
        const auto& vertex0 = p_mesh.m_vertices[index0].Position * scale;
        const auto& vertex1 = p_mesh.m_vertices[index1].Position * scale;
        const auto& vertex2 = p_mesh.m_vertices[index2].Position * scale;

        // Add the triangle to the mesh with reversed winding order (flip normals)
        invertedTriangleMesh->addTriangle(
            btVector3(vertex0.x, vertex0.y, vertex0.z),
            btVector3(vertex2.x, vertex2.y, vertex2.z), // Swap these two
            btVector3(vertex1.x, vertex1.y, vertex1.z)
        );
    }

    // Create the triangle mesh shape
    btBvhTriangleMeshShape* invertedTriangleMeshShape = new btBvhTriangleMeshShape(invertedTriangleMesh, true);

    // Store the triangle mesh in the user pointer
    invertedTriangleMeshShape->setUserPointer(invertedTriangleMesh);
    m_collisionShapes.push_back(invertedTriangleMeshShape);
    return invertedTriangleMeshShape;
}



void CollisionManager::AddShape(btCollisionShape* p_newShape)
{
    if (p_newShape != nullptr)
    {
        m_collisionShapes.push_back(p_newShape);
    }
}

CollisionManager::~CollisionManager()
{
    // Clean up collision objects in the dynamics world
    for (int i = m_dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
    {
        btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[i];
        btRigidBody* body = btRigidBody::upcast(obj);

        // Delete motion state if it exists
        if (body && body->getMotionState())
        {
            delete body->getMotionState();
        }

        m_dynamicsWorld->removeCollisionObject(obj);
        delete obj;
    }

    // Track already-deleted auxiliary resources
    std::unordered_set<void*> deletedMeshes;

    // Clean up collision shapes
    for (int j = 0; j < m_collisionShapes.size(); j++)
    {
        btCollisionShape* shape = m_collisionShapes[j];
        void* userPointer = shape->getUserPointer();
        // if the shape is a mesh make sure to delete it
        if (userPointer && deletedMeshes.find(userPointer) == deletedMeshes.end())
        {
            btTriangleMesh* triangleMesh = static_cast<btTriangleMesh*>(userPointer);

            if (reinterpret_cast<uintptr_t>(triangleMesh) % alignof(btTriangleMesh) == 0 && triangleMesh != reinterpret_cast<void*>(-1))
            {
                deletedMeshes.insert(userPointer);
                delete shape;
                delete triangleMesh;
            }
        }
    }

    // these objects need to be deleted in a difrent loop since otherwise there is a memory 
    // leak or the game crashes
    for (auto& shape : m_initializedShapes)
    {
        delete shape;
    }

    // Clear the collision shapes array
    m_collisionShapes.clear();

    // Delete dynamics world components
    delete m_dynamicsWorld;
    delete m_solver;
    delete m_overlappingPairCache;
    delete m_dispatcher;
    delete m_collisionConfiguration;

    // Delete other managed resources
    delete m_lineShader;
    delete m_debugDrawer;
    delete m_triggerManager;
}
