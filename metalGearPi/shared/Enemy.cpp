#include "Enemy.h"

#include "camera.h"
#include "collisionManager.h"
#include "EnemyManager.h"
#include "modelInstance.h"
#include "Random.h"
#include "player.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp> 

Enemy::Enemy(Camera& p_camera, ModelInstance& p_modelIns, ModelInstance& p_rifleModelIns, CollisionManager& p_collisionManager, 
	EnemyManager& p_enemyManager, const glm::vec3& p_playerPos, uint& p_seed, SceneGraphNode& p_parentNode)
	: m_collisionManager(p_collisionManager), BaseUpdatable(p_camera), m_enemyManager(p_enemyManager), 
	  m_playerPos(p_playerPos), m_seed(p_seed)
{
	m_instance = new Instance
	(
		glm::vec3(1.0f),
		glm::vec3(1.0f),
		glm::vec3(3.5f),
		p_parentNode.m_rootTrans,
		true,
		new RigidBody(
			*m_collisionManager.m_dynamicsWorld,
			m_collisionManager.GetShape(2),
			btVector3(0.0f, -10.0f, 0.0f),
			100.0f,
			CollisionManager::CollisionMask::Enemy,
			CollisionManager::CollisionMask::All & ~CollisionManager::CollisionMask::Enemy
		)
	);

	Instance* gunInstance = new Instance
	(
		glm::vec3(0.2f, 0.1f, -0.1f),
		glm::vec3(0.0f, 180.0f, 0.0f),
		glm::vec3(1.5f),
		p_parentNode.m_rootTrans,
		false,
		nullptr
	);

	p_modelIns.AddInstance(m_instance);
	p_parentNode.m_childeren.push_back(dynamic_cast<SceneGraphNode*>(m_instance));

	p_rifleModelIns.AddInstance(gunInstance);
	m_instance->m_childeren.push_back(dynamic_cast<SceneGraphNode*>(gunInstance));


	m_instance->rigidBody->m_body->setAngularFactor(btVector3(0, 0, 0));
	m_instance->rigidBody->m_body->setActivationState(DISABLE_SIMULATION);
	m_instance->rigidBody->m_body->setCollisionFlags(m_instance->rigidBody->m_body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);

	m_instance->rigidBody->m_body->setUserPointer(this);
}

void Enemy::Update(float p_deltatime)
{
	if (m_isDead) { return; }


	if (m_enemyState == EnemyState::patrolling)
	{
		if (CheckIfPlayerIsSeen())
		{
			m_enemyManager.AlertEnemies();
		}
	}


	switch (m_enemyState)
	{
		case Enemy::EnemyState::alert:
		{
			glm::vec3 moveDir = glm::normalize(m_playerPos - m_instance->pos);
			LookAtDir(glm::vec2(moveDir.x, moveDir.z));

			if (glm::distance(m_playerPos, m_instance->pos) > DISTANCE_FROM_PLAYER || !CanSeePlayer(moveDir))
			{
				moveDir = GetDirAvoidingObstacles(moveDir);
				Move(glm::vec2(moveDir.x, moveDir.z));
			}
			else
			{
				m_instance->rigidBody->m_body->setLinearVelocity(btVector3(btVector3(0.0f, 0.0f, 0.0f)));
				if (m_shootTimer <= 0.0f)
				{
					m_shootTimer = SHOOT_COUNTDOWN * RandomFloat(m_seed) + 1.0f;
					Shoot();
				}
				else
				{
					m_shootTimer -= p_deltatime;
				}
			}
			

			break;
		}
		default:
		case Enemy::EnemyState::patrolling:
		{
			if (m_patrollPoints.empty()) { return; }


			glm::vec2 pos = glm::vec2(m_instance->pos.x, m_instance->pos.z);
			glm::vec2 target = glm::vec2(m_patrollPoints[m_patrolIndex].x, m_patrollPoints[m_patrolIndex].z);
			if (m_patrollPoints.size() < 2)
			{
				LookAtDir(glm::normalize(target - pos));

				return;
			}

			// Edge case: If the current position matches the target position
			if (glm::distance(pos, target) < std::numeric_limits<float>::epsilon())
			{
				m_patrolIndex++;
				if (m_patrolIndex >= m_patrollPoints.size())
				{
					m_patrolIndex = 0;
				}
				return; // Return early
			}

			glm::vec2 moveDir = glm::normalize(target - pos);
			LookAtDir(moveDir);
			Move(moveDir);

			if (glm::distance(pos, target) < DISTANCE_FROM_PATROL_POINT)
			{
				m_patrolIndex++;
				if (m_patrolIndex >= m_patrollPoints.size())
				{
					m_patrolIndex = 0;
				}
			}
			break;
		}

	}



}

void Enemy::Render()
{
}

void Enemy::Die()
{
	m_isDead = true;
	m_instance->m_active = false;
	m_instance->rigidBody->m_body->setActivationState(DISABLE_SIMULATION);
	m_instance->rigidBody->m_body->setCollisionFlags(m_instance->rigidBody->m_body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
	//m_instance->rigidBody->m_body->setActivationState(DISABLE_SIMULATION);
	m_instance->SetPos(DEFAULT_POS);
}

void Enemy::Spawn(glm::vec3 p_pos)
{
	m_health = MAX_HEALTH;
	m_shootTimer = SHOOT_COUNTDOWN * RandomFloat(m_seed);

	m_isDead = false;
	m_instance->SetPos(p_pos);
	m_instance->m_active = true;

	m_instance->rigidBody->m_body->setCollisionFlags(m_instance->rigidBody->m_body->getCollisionFlags() & ~btCollisionObject::CF_NO_CONTACT_RESPONSE);
	m_instance->rigidBody->m_body->forceActivationState(ACTIVE_TAG);

}

bool Enemy::CheckIfPlayerIsSeen()
{
	glm::vec3 origin = m_instance->pos; // Enemy's position

	// Assuming m_instance->modelMatrix is the enemy's model matrix
	glm::mat4 modelMatrix = m_instance->m_worldTrans;  // Get the model matrix

	// Directions derived from the model matrix
	glm::vec3 forwardDir = glm::normalize(glm::vec3(modelMatrix[2]));  // Forward (Z-axis)
	glm::vec3 rightDir = glm::normalize(glm::vec3(modelMatrix[0]));    // Right (X-axis)
	glm::vec3 upDir = glm::normalize(glm::vec3(modelMatrix[1]));       // Up (Y-axis)

	//printf("forward %f %f %f \n", forwardDir.x, forwardDir.y, forwardDir.z);

	if (true)
	{

	}

	float rayLength = 25.0f;         // Length of the ray
	float horizontalOffset = 2.7f;   // Offset to the left/right

	// Create ray end points directly
	std::vector<glm::vec3> rayDirections = {
		origin + forwardDir * rayLength,                                // Center ray
		origin + (forwardDir * rayLength) + (rightDir * horizontalOffset),  // Right ray
		origin + (forwardDir * rayLength) - (rightDir * horizontalOffset),  // Left ray
		origin + (forwardDir * rayLength) + (rightDir * (horizontalOffset * 2)),  // Right ray
		origin + (forwardDir * rayLength) - (rightDir * (horizontalOffset * 2)),  // Left ray
	};


	for (const auto& dir : rayDirections)
	{
		//glm::vec3 rayEnd = origin + dir * rayLength;
		glm::vec3 rayEnd = dir;

		btVector3 btRayStart(origin.x, origin.y, origin.z);
		btVector3 btRayEnd(rayEnd.x, rayEnd.y, rayEnd.z);

		btCollisionWorld::ClosestRayResultCallback rayCallback(btRayStart, btRayEnd);
		rayCallback.m_collisionFilterMask = CollisionManager::CollisionMask::Player | CollisionManager::CollisionMask::Environment;
		m_collisionManager.m_dynamicsWorld->rayTest(btRayStart, btRayEnd, rayCallback);

		// Visualize the ray in debug drawer
		btVector3 startColor(0.0f, 1.0f, 0.0f); // Green for no hit
		btVector3 endColor(1.0f, 0.0f, 0.0f);   // Red if it hits something


		if (rayCallback.hasHit())
		{
			m_collisionManager.m_dynamicsWorld->getDebugDrawer()->drawLine(btRayStart, rayCallback.m_hitPointWorld, endColor);

			// Check if the hit object belongs to the Player group
			const btCollisionObject* hitObject = rayCallback.m_collisionObject;
			if (hitObject)
			{
				int objectGroup = hitObject->getBroadphaseHandle()->m_collisionFilterGroup;
				if (objectGroup & CollisionManager::CollisionMask::Player)
				{
					return true;
				}
			}
		}
		else
		{
			m_collisionManager.m_dynamicsWorld->getDebugDrawer()->drawLine(btRayStart, btRayEnd, startColor);
		}
	}

	return false;
}

void Enemy::TakeDamage(float p_damage)
{
	m_health -= p_damage;

	if (m_health < 1.0f)
	{
		Die();
	}

}

glm::vec3 Enemy::GetDirAvoidingObstacles(const glm::vec3& p_moveDir)
{
	glm::vec3 finalOffset = glm::vec3(0.0f);

	// Define the ray length
	float rayLength = 7.0f;
	glm::vec3 rayStart = m_instance->pos;
	glm::vec3 rayEnd = rayStart + p_moveDir * rayLength;

	btVector3 btRayStart(rayStart.x, rayStart.y, rayStart.z);
	btVector3 btRayEnd(rayEnd.x, rayEnd.y, rayEnd.z);

	// Perform the raycast to check for obstacles (main ray)
	btCollisionWorld::ClosestRayResultCallback rayCallback(btRayStart, btRayEnd);
	rayCallback.m_collisionFilterMask = CollisionManager::CollisionMask::Environment;
	m_collisionManager.m_dynamicsWorld->rayTest(btRayStart, btRayEnd, rayCallback);

	// Debug draw the main ray
	if (m_collisionManager.m_dynamicsWorld->getDebugDrawer())
	{
		m_collisionManager.m_dynamicsWorld->getDebugDrawer()->drawLine(btRayStart, btRayEnd, btVector3(1.0f, 0.0f, 0.0f));  // Red line for the main ray
	}

	// Cast rays to the left and right regardless of whether the main ray hits or not
	glm::vec3 leftOffset = glm::normalize(glm::cross(p_moveDir, glm::vec3(0.0f, 10.0f, 0.0f))) * 2.0f;
	glm::vec3 rightOffset = -leftOffset;

	glm::vec3 leftRayEnd = rayStart + p_moveDir * rayLength + leftOffset;
	glm::vec3 rightRayEnd = rayStart + p_moveDir * rayLength + rightOffset;

	btVector3 btLeftRayEnd(leftRayEnd.x, leftRayEnd.y, leftRayEnd.z);
	btVector3 btRightRayEnd(rightRayEnd.x, rightRayEnd.y, rightRayEnd.z);

	btCollisionWorld::ClosestRayResultCallback leftRayCallback(btRayStart, btLeftRayEnd);
	btCollisionWorld::ClosestRayResultCallback rightRayCallback(btRayStart, btRightRayEnd);

	leftRayCallback.m_collisionFilterMask = CollisionManager::CollisionMask::Environment;
	rightRayCallback.m_collisionFilterMask = CollisionManager::CollisionMask::Environment;

	// Perform the raycasts for left and right
	m_collisionManager.m_dynamicsWorld->rayTest(btRayStart, btLeftRayEnd, leftRayCallback);
	m_collisionManager.m_dynamicsWorld->rayTest(btRayStart, btRightRayEnd, rightRayCallback);

	// Debug draw the left and right offset rays
	if (m_collisionManager.m_dynamicsWorld->getDebugDrawer())
	{
		m_collisionManager.m_dynamicsWorld->getDebugDrawer()->drawLine(btRayStart, btLeftRayEnd, btVector3(0.0f, 1.0f, 0.0f));  // Green line for left offset ray
		m_collisionManager.m_dynamicsWorld->getDebugDrawer()->drawLine(btRayStart, btRightRayEnd, btVector3(0.0f, 0.0f, 1.0f));  // Blue line for right offset ray
	}

	// Add a smooth blending factor for the raycast hits
	float smoothFactor = 0.2f;  // Adjust this factor for more or less smoothing

	// If neither ray hit, continue in the original direction
	if (!leftRayCallback.hasHit() && !rightRayCallback.hasHit())
	{
		return p_moveDir;  // No hits, continue in the original direction
	}
	else if (!leftRayCallback.hasHit())
	{
		// Left side is clear, move to the left with smoothing
		finalOffset = glm::normalize(glm::cross(p_moveDir, glm::vec3(0.0f, 1.0f, 0.0f))) * 2.0f;
		return glm::mix(p_moveDir, p_moveDir + finalOffset, smoothFactor);  // Smooth the transition
	}
	else if (!rightRayCallback.hasHit())
	{
		// Right side is clear, move to the right with smoothing
		finalOffset = -glm::normalize(glm::cross(p_moveDir, glm::vec3(0.0f, 1.0f, 0.0f))) * 2.0f;
		return glm::mix(p_moveDir, p_moveDir + finalOffset, smoothFactor);  // Smooth the transition
	}

	// If both sides are blocked, just return the original direction (no movement)
	return p_moveDir;
}


bool Enemy::CanSeePlayer(glm::vec3 p_dir)
{
	glm::vec3 finalOffset = glm::vec3(0.0f);

	// Define the ray length
	float rayLength = 20.0f;
	glm::vec3 rayStart = m_instance->pos;
	glm::vec3 rayEnd = rayStart + p_dir * rayLength;

	btVector3 btRayStart(rayStart.x, rayStart.y, rayStart.z);
	btVector3 btRayEnd(rayEnd.x, rayEnd.y, rayEnd.z);

	// Perform the raycast to check for obstacles
	btCollisionWorld::ClosestRayResultCallback rayCallback(btRayStart, btRayEnd);
	rayCallback.m_collisionFilterMask = CollisionManager::CollisionMask::Player | CollisionManager::CollisionMask::Environment;
	m_collisionManager.m_dynamicsWorld->rayTest(btRayStart, btRayEnd, rayCallback);

	// Debug draw the main ray
	if (m_collisionManager.m_dynamicsWorld->getDebugDrawer())
	{
		m_collisionManager.m_dynamicsWorld->getDebugDrawer()->drawLine(btRayStart, btRayEnd, btVector3(1.0f, 0.0f, 0.0f));  // Red line for the main ray
	}

	const btCollisionObject* hitObject = rayCallback.m_collisionObject;
	if (hitObject)
	{
		int objectGroup = hitObject->getBroadphaseHandle()->m_collisionFilterGroup;
		if (objectGroup & CollisionManager::CollisionMask::Player)
		{
			return true;
		}
	}

	return false;
}

void Enemy::Shoot()
{
	printf("shot at player \n");

	// Random offsets for spread
	float offSetX = 7.0f * RandomFloat(m_seed) - 3.5f;
	float offSetY = 7.0f * RandomFloat(m_seed) - 3.0f;
	glm::vec3 offset = glm::vec3(offSetX, offSetY, 0.0f);

	// Ray length
	float rayLength = 30.0f;

	// Calculate the direction towards the player and add the random offset
	glm::vec3 targetDir = glm::normalize(m_playerPos - m_instance->pos);
	glm::vec3 rayEnd = m_instance->pos + targetDir * rayLength + offset;

	// Convert to Bullet physics vector
	btVector3 btRayStart(m_instance->pos.x, m_instance->pos.y, m_instance->pos.z);
	btVector3 btRayEnd(rayEnd.x, rayEnd.y, rayEnd.z);

	// Raycast to check for obstacles
	btCollisionWorld::ClosestRayResultCallback rayCallback(btRayStart, btRayEnd);
	rayCallback.m_collisionFilterMask = CollisionManager::CollisionMask::Player | CollisionManager::CollisionMask::Environment;
	m_collisionManager.m_dynamicsWorld->rayTest(btRayStart, btRayEnd, rayCallback);

	btVector3 hitColor(1.0f, 0.0f, 0.0f);  // Red for hit
	btVector3 missColor(0.0f, 1.0f, 0.0f); // Green for miss

	// Check if the ray hit something
	if (rayCallback.hasHit())
	{
		// Draw the line from the start to the hit point
		m_collisionManager.m_dynamicsWorld->getDebugDrawer()->drawLine(btRayStart, rayCallback.m_hitPointWorld, hitColor);

		const btCollisionObject* hitObject = rayCallback.m_collisionObject;
		if (hitObject)
		{
			int objectGroup = hitObject->getBroadphaseHandle()->m_collisionFilterGroup;
			if (objectGroup & CollisionManager::CollisionMask::Player)
			{
				// Hit the player, apply damage
				Player* player = static_cast<Player*>(hitObject->getUserPointer());
				if (player)
				{
					player->TakeDamage(20.0f);
				}
				return;
			}
		}
	}
	else
	{
		// Ray didn't hit anything, draw the ray to the end
		m_collisionManager.m_dynamicsWorld->getDebugDrawer()->drawLine(btRayStart, btRayEnd, missColor);
	}
}

void Enemy::Move(glm::vec2 p_dir)
{
	m_instance->rigidBody->m_body->activate(true);
	glm::vec2 movement = p_dir * ENEMY_MOVE_SPEED;
	m_instance->rigidBody->m_body->setLinearVelocity(btVector3(btVector3(movement.x, 0.0f, movement.y)));
}

void Enemy::LookAtDir(glm::vec2 p_dir)
{
	float yaw = atan2(p_dir.x, p_dir.y); // Correct for a flat XZ-plane
	glm::quat newRotation = glm::quat(glm::vec3(0.0f, yaw, 0.0f));
	m_instance->rot = glm::degrees(glm::eulerAngles(newRotation));
}

Enemy::~Enemy()
{
	if (m_instance != nullptr)
	{
		//delete m_instance;
	}
}