#include "sceneGraph.h"


#ifdef LINUX_BUILD
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#endif // LINUX_BUILD

#ifdef WINDOWS_BUILD
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#endif // WINDOWS_BUILD

SceneGraphNode::SceneGraphNode(const glm::mat4* p_rootTrans, const glm::mat4 p_localTrans, bool p_ignoreParent)
	: m_rootTrans(p_rootTrans), m_localTrans(p_localTrans), m_ignoreParentTranslation(p_ignoreParent)
{
	m_worldTrans = m_localTrans;
}

SceneGraphNode::~SceneGraphNode()
{
	//for (auto& child : m_childeren)
	//{
	//	if (child != nullptr)
	//	{
	//		delete child;
	//	}
	//}
	m_childeren.clear();
}

void SceneGraphNode::UpdateMat(const glm::mat4& p_parentSGN)
{
	if (!m_ignoreParentTranslation)
	{
		m_worldTrans = *m_rootTrans * p_parentSGN * m_localTrans;
	}
	else
	{
		m_worldTrans = m_localTrans;
	}

	for (auto& child : m_childeren)
	{
		child->UpdateMat(m_worldTrans);
	}
}

void SceneGraphNode::SetToDefaultLocalMat()
{
	glm::vec3 position(0.0f, 0.0f, 0.0f);
	glm::vec3 rotation(0.0f, 0.0f, 0.0f);
	glm::vec3 scale(1.0f, 1.0f, 1.0f);

	m_localTrans = glm::translate(glm::mat4(1.0f), position) *
		glm::rotate(glm::mat4(1.0f), rotation.x, glm::vec3(1.0f, 0.0f, 0.0f)) *
		glm::rotate(glm::mat4(1.0f), rotation.y, glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::rotate(glm::mat4(1.0f), rotation.z, glm::vec3(0.0f, 0.0f, 1.0f)) *
		glm::scale(glm::mat4(1.0f), scale);
}

SceneGraphRoot::SceneGraphRoot()
{
	glm::vec3 position(0.0f, 0.0f, 0.0f);
	glm::vec3 rotation(0.0f, 0.0f, 0.0f);
	glm::vec3 scale(1.0f, 1.0f, 1.0f);

	m_worldTrans = glm::translate(glm::mat4(1.0f), position) *
		glm::rotate(glm::mat4(1.0f), rotation.x, glm::vec3(1.0f, 0.0f, 0.0f)) *
		glm::rotate(glm::mat4(1.0f), rotation.y, glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::rotate(glm::mat4(1.0f), rotation.z, glm::vec3(0.0f, 0.0f, 1.0f)) *
		glm::scale(glm::mat4(1.0f), scale);
}

SceneGraphRoot::~SceneGraphRoot()
{
	//for (auto& child : m_childeren)
	//{
	//	if (child != nullptr)
	//	{
	//		delete child;
	//	}
	//}
	m_childeren.clear();
}

void SceneGraphRoot::UpdateMat()
{
	for (auto& child : m_childeren)
	{
		child->UpdateMat(m_worldTrans);
	}
}
