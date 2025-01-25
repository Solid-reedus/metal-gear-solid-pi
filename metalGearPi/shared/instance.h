#pragma once

#ifdef LINUX_BUILD
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#endif // LINUX_BUILD

#ifdef WINDOWS_BUILD
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#endif // WINDOWS_BUILD

#include "rigidBody.h"
#include <vector>
#include "sceneGraph.h"

struct Instance : public SceneGraphNode
{
	glm::vec3 pos;
	glm::vec3 rot;
	glm::vec3 scl;
	//glm::mat4 trans;

	RigidBody* rigidBody = nullptr;
	bool m_active = true;

	Instance() : SceneGraphNode(nullptr, glm::mat4(1.0f), true)
	{
		pos = glm::vec3(0.0f);
		rot = glm::vec3(0.0f);
		scl = glm::vec3(0.0f);
		//trans = glm::mat4(1.0f);
	}

	Instance(glm::vec3 p_pos, glm::vec3 p_rot, glm::vec3 p_scl, const glm::mat4* p_rootTrans, bool p_ignoreParent = false, RigidBody* p_rigidBody = nullptr)
		: SceneGraphNode(p_rootTrans, glm::mat4(1.0f), p_ignoreParent)
	{
		pos = p_pos;
		rot = p_rot;
		scl = p_scl;

		m_localTrans = glm::mat4(1.0f);

		m_localTrans = glm::scale(m_localTrans, scl);

		m_localTrans = glm::rotate(m_localTrans, glm::radians(rot.x), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate around X-axis
		m_localTrans = glm::rotate(m_localTrans, glm::radians(rot.y), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate around Y-axis
		m_localTrans = glm::rotate(m_localTrans, glm::radians(rot.z), glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate around Z-axis

		m_localTrans = glm::translate(m_localTrans, pos);
		m_worldTrans = m_localTrans;

		rigidBody = p_rigidBody;
	}

	~Instance()
	{
		if (rigidBody != nullptr)
		{
			delete rigidBody;
		}
	}

	// this funtion makes sure that the model matrix is the same as the rigid body
	void UpdateModelMatrix()
	{
		if (rigidBody == nullptr)
		{
			return;
		}

		btTransform btTrans;
		rigidBody->m_body->getMotionState()->getWorldTransform(btTrans);
		btVector3 origin = btTrans.getOrigin();

		glm::mat4 rotationMatrix = glm::mat4(1.0f);
		rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rot.x), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate around X-axis
		rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rot.y), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate around Y-axis
		rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rot.z), glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate around Z-axis

		glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(origin.getX(), origin.getY(), origin.getZ()));
		glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(scl));
		m_localTrans = translationMatrix * rotationMatrix * scaleMatrix;

		pos = glm::vec3(origin.getX(), origin.getY(), origin.getZ());

	}

	Instance(const glm::mat4& p_trans, const glm::mat4* p_rootTrans, bool p_ignoreParent = false, RigidBody* p_rigidBody = nullptr)
		: SceneGraphNode(p_rootTrans, glm::mat4(1.0f), p_ignoreParent)
	{
		m_localTrans = glm::mat4(p_trans);
		rigidBody = p_rigidBody;
		m_worldTrans = m_localTrans;

		pos = glm::vec3(p_trans[3]); // Extract translation
		scl = glm::vec3(
			glm::length(glm::vec3(p_trans[0])),
			glm::length(glm::vec3(p_trans[1])),
			glm::length(glm::vec3(p_trans[2]))
		);

		glm::mat3 rotationMatrix = glm::mat3(
			glm::vec3(p_trans[0]) / scl.x,
			glm::vec3(p_trans[1]) / scl.y,
			glm::vec3(p_trans[2]) / scl.z
		);

		rot = glm::degrees(glm::eulerAngles(glm::quat_cast(rotationMatrix)));
	}
	
	void SetPos(const glm::vec3& p_newPos)
	{
		btTransform btTrans;
		rigidBody->m_body->getMotionState()->getWorldTransform(btTrans);
		btTrans.setOrigin(btVector3(p_newPos.x, p_newPos.y, p_newPos.z));
		rigidBody->m_body->setWorldTransform(btTrans);
		rigidBody->m_body->getMotionState()->setWorldTransform(btTrans);
		rigidBody->m_body->setLinearVelocity(btVector3(0, 0, 0));   // No linear velocity
		rigidBody->m_body->setAngularVelocity(btVector3(0, 0, 0));  // No angular velocity

		UpdateModelMatrix();
	}


};