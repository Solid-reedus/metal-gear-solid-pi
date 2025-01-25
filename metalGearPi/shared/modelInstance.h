#pragma once

#include "model.h"
#include "camera.h"

class ModelInstance
{
	public:
	ModelInstance(Model* p_model, BaseShader* p_shader, bool p_isInverted = false, CollisionManager* p_collisionManager = nullptr) : m_model(p_model), m_shader(p_shader)
	{

		if (p_collisionManager != nullptr)
		{
			if (p_isInverted)
			{
				m_usedShape = p_collisionManager->CreateInvertedMeshCollider(m_model->LookAtMeshes()[0]);
			}
			else
			{
				m_usedShape = p_collisionManager->CreateMeshCollider(m_model->LookAtMeshes()[0], false);
			}
			p_collisionManager->AddShape(m_usedShape);
		}

	};

	~ModelInstance()
	{
		delete m_model;
		for (auto& ins : m_instances)
		{
			delete ins;
		}
	}

	// need to pass A view and a projection mat4
	void Draw(const Camera& p_cam)
	{
		glm::mat4 viewProj = p_cam.GetProjectionMatrix() * p_cam.GetViewMatrix();
		for (Instance* ins : m_instances)
		{
			if (!ins->m_active)
			{
				continue;
			}

			ins->UpdateModelMatrix();
			m_shader->use();
			m_shader->setMat4("MVP", viewProj * ins->m_worldTrans);
			m_shader->setMat4("model", ins->m_worldTrans);
			m_model->Draw(dynamic_cast<BaseShader*>(m_shader));
		}
	}

	void AddInstance(Instance* p_ins)
	{
		m_instances.push_back(p_ins);
	}

	void PopInstance(Instance* p_ins)
	{
		auto it = std::find(m_instances.begin(), m_instances.end(), p_ins);
		if (it != m_instances.end())
		{
			m_instances.erase(it); // Remove the instance if found
		}
	}

	void Clear()
	{
		for (auto& ins : m_instances)
		{
			delete ins;
		}

		m_instances.clear();
		//m_sceneGraphNode->m_childeren.clear();
	}

	const Model* LookAtModel() { return m_model; }
	const Instance* LookAtInstance(int p_index) 
	{
		if (p_index < m_instances.size())
		{
			return m_instances[p_index];
		}
		return nullptr;
		//return (p_index >= m_instances.size()) ? m_instances[p_index] : nullptr;
	}


	btCollisionShape* m_usedShape = nullptr;
	//SceneGraphNode* m_sceneGraphNode = nullptr;


	private:
	Model* m_model = nullptr;
	std::vector<Instance*> m_instances;
	BaseShader* m_shader = nullptr;
};

