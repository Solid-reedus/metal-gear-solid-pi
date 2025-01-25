#pragma once


#include "baseUpdatable.h"
#include "common.h"

class BaseShader;
class CollisionManager;
class ModelInstance;
class EnemyManager;

class Enemy : public BaseUpdatable
{
	public:

	enum class EnemyState
	{
		patrolling,
		alert
	};


	Enemy(Camera& p_camera, ModelInstance& p_modelIns, ModelInstance& p_rifleModelIns, CollisionManager& p_collisionManager, EnemyManager& p_enemyManager, const glm::vec3& p_playerPos, uint& p_seed, SceneGraphNode& p_parentNode);
	void Update(float p_deltatime) override;
	void Render() override;
	~Enemy();

	void Die();
	void Spawn(glm::vec3 p_pos);
	void GoIntoState(EnemyState p_newState) { m_enemyState = p_newState; }
	const bool IsDead() { return m_isDead; }
	bool CheckIfPlayerIsSeen();
	void SetPatrolPoints(const std::vector<glm::vec3> p_patrollPoints) { m_patrollPoints = p_patrollPoints, m_patrolIndex = 0; }
	void TakeDamage(float p_damage);

	private:

	const glm::vec3 DEFAULT_POS = glm::vec3(0.0f, -10.0f, 0.0f);
	const float DISTANCE_FROM_PLAYER = 20.0f;
	const float DISTANCE_FROM_PATROL_POINT = 1.1f;
	const float ENEMY_MOVE_SPEED = 5.0f;
	const float MAX_HEALTH = 100.0f;
	const float SHOOT_COUNTDOWN = 3.0f;

	void Move(glm::vec2 p_dir);
	void LookAtDir(glm::vec2 p_dir);
	glm::vec3 GetDirAvoidingObstacles(const glm::vec3& p_moveDir);
	bool CanSeePlayer(glm::vec3 p_dir);
	void Shoot();

	CollisionManager& m_collisionManager;
	EnemyManager& m_enemyManager;
	EnemyState m_enemyState = EnemyState::patrolling;
	Instance* m_instance = nullptr;
	bool m_isDead = true;
	const glm::vec3& m_playerPos;
	float m_shootTimer = 0.0f;

	uint& m_seed;

	std::vector<glm::vec3> m_patrollPoints;
	int m_patrolIndex = 0;

	float m_health = 0.0f;

};

