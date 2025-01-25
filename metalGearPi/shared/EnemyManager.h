#pragma once

#ifdef LINUX_BUILD
#include <glm/glm.hpp>
#endif // LINUX_BUILD

#ifdef WINDOWS_BUILD
#include "glm.hpp"
#endif // WINDOWS_BUILD

#include "common.h"
#include <vector>

#define ENEMY_COUNT 5

class CollisionManager;
class LightingManager;
class SceneGraphNode;
class ModelInstance;
class BaseShader;
struct Camera;
class Enemy;


class EnemyManager
{
	public:
	EnemyManager(Camera& p_camera, CollisionManager& p_collisionManager, LightingManager& p_lightingManager, BaseShader* p_shader, const glm::vec3& p_playerPos, uint& p_seed, SceneGraphNode& p_parentNode);
	~EnemyManager();

	void DisableEnemies();
	void SpawnEnemy(glm::vec3 p_pos, std::vector<glm::vec3> p_patrolPoints);
	void Render();
	void Update(float p_deltatime);
	void AlertEnemies();

	//set funtions
	ModelInstance* m_enemyModel;
	ModelInstance* m_rifleModel;

	SceneGraphNode* m_sceneGraphNode = nullptr;

	private:
	Enemy* m_enemies[ENEMY_COUNT];
	Camera& m_camera;
	LightingManager& m_lightingManager;
	
	int m_enemyIndex = 0;
};

