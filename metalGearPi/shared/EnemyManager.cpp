#include "EnemyManager.h"

#include "Enemy.h"
#include "modelInstance.h"
#include "common.h"
#include <vector>
#include "lightingManager.h"

#ifdef LINUX_BUILD
#include <glm/glm.hpp>
#endif // LINUX_BUILD

#ifdef WINDOWS_BUILD
#include "glm.hpp"
#endif // WINDOWS_BUILD


EnemyManager::EnemyManager(Camera& p_camera, CollisionManager& p_collisionManager, LightingManager& p_lightingManager, BaseShader* p_shader, const glm::vec3& p_playerPos, uint& p_seed, SceneGraphNode& p_parentNode)
	: m_lightingManager(p_lightingManager), m_camera(p_camera)
{
	m_sceneGraphNode = new SceneGraphNode(p_parentNode.m_rootTrans, glm::mat4(1.0f), false);
	m_sceneGraphNode->SetToDefaultLocalMat();
	p_parentNode.m_childeren.push_back(m_sceneGraphNode);

	m_enemyModel = new ModelInstance(new Model(std::string(ASSETS_DIR) + "snake/playerModel.model"), p_shader);
	m_rifleModel = new ModelInstance(new Model(std::string(ASSETS_DIR) + "weapons/rifle/akRifle2.obj"), p_shader);

	for (int i = 0; i < ENEMY_COUNT; i++)
	{
		m_enemies[i] = new Enemy(p_camera, *m_enemyModel, *m_rifleModel, p_collisionManager, *this, p_playerPos, p_seed, *m_sceneGraphNode);
	}
}

EnemyManager::~EnemyManager()
{
	delete m_enemyModel;
	delete m_rifleModel;

	for (auto& enemy : m_enemies)
	{
		if (enemy != nullptr)
		{
			delete enemy;
		}
	}
	delete m_sceneGraphNode;

}

void EnemyManager::DisableEnemies()
{
	for (auto& enemy : m_enemies)
	{
		enemy->GoIntoState(Enemy::EnemyState::patrolling);
		enemy->Die();
	}
	m_enemyIndex = 0;

	m_lightingManager.ClearSpotLights();
}

void EnemyManager::SpawnEnemy(glm::vec3 p_pos, std::vector<glm::vec3> p_patrolPoints)
{
	if (!m_enemies[m_enemyIndex]->IsDead())
	{
		return;
	}

	m_lightingManager.AddSpotLight(LightingManager::SpotLight
	(
		glm::vec3(1.0f),
		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(1.0f),
		2.0f
	));


	m_enemies[m_enemyIndex]->SetPatrolPoints(p_patrolPoints);
	m_enemies[m_enemyIndex]->Spawn(p_pos);



	m_enemyIndex++;
	if (m_enemyIndex > ENEMY_COUNT)
	{
		m_enemyIndex = 0;
	}

}

void EnemyManager::Render()
{
	m_enemyModel->Draw(m_camera);
	m_rifleModel->Draw(m_camera);
}

void EnemyManager::Update(float p_deltatime)
{
	int i = 0;
	for (auto& enemy : m_enemies)
	{
		if (enemy->IsDead())
		{
			m_lightingManager.UpdateLight(i, LightingManager::LightType::spotlight, LightingManager::LightAtr::intensity, glm::vec3(0.0f));
			continue;
		}
		enemy->Update(p_deltatime);

		const Instance* enemyIns = m_enemyModel->LookAtInstance(i);
		glm::vec3 forward = glm::vec3(-enemyIns->m_worldTrans[0][2], enemyIns->m_worldTrans[1][2], enemyIns->m_worldTrans[2][2]);
		glm::vec3 position = glm::vec3(enemyIns->m_worldTrans[3]);
		position.y += 1.5f;
		//position.x = 20.0f;
		m_lightingManager.UpdateLight(i, LightingManager::LightType::spotlight, LightingManager::LightAtr::pos, position);
		m_lightingManager.UpdateLight(i, LightingManager::LightType::spotlight, LightingManager::LightAtr::dir, forward);

		i++;
	}
}

void EnemyManager::AlertEnemies()
{
	for (auto& enemy : m_enemies)
	{
		enemy->GoIntoState(Enemy::EnemyState::alert);
	}
}
