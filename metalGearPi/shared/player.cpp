#include "player.h"
#include "baseUpdatable.h"
#include "modelInstance.h"
#include "camera.h"
#include "common.h"
#include "instance.h"
#include "collisionManager.h"
#include "baseInput.h"
#include "Enemy.h"
#include "UIRectangle.h"
#include "mat3Functions.h"
#include "ShaderManager.h"

Player::Player(Camera& p_camera, ShaderManager& p_shaderManager, CollisionManager& p_collisionManager, SceneGraphRoot& p_sceneGraphRoot)
	: m_camera(p_camera), m_collisionManager(p_collisionManager), BaseUpdatable(p_camera)
{
	glm::vec3 playerPos = glm::vec3(0.0f, 8.0f, -6.0f);
	p_camera.front = glm::normalize(glm::vec3(0.0f, -1.0f, 2.5f));

	m_model = new ModelInstance(new Model(std::string(ASSETS_DIR) + "snake/playerModel.model"), p_shaderManager.GetShader(ShaderManager::ShaderEnum::defaultShader));
	m_pistol = new ModelInstance(new Model(std::string(ASSETS_DIR) + "weapons/pistol/pistol.obj"), p_shaderManager.GetShader(ShaderManager::ShaderEnum::defaultShader));
	

	m_playerInstance = new Instance
	(
		playerPos,
		glm::vec3(0.0f),
		glm::vec3(3.5f),
		&p_sceneGraphRoot.m_worldTrans,
		true,
		new RigidBody(
			*m_collisionManager.m_dynamicsWorld,
			m_collisionManager.GetShape(2),
			btVector3(playerPos.x, playerPos.y, playerPos.z), 
			100.0f,
			CollisionManager::CollisionMask::Player,
			CollisionManager::CollisionMask::All
		)
	);

	m_pistolInstanceAiming = new Instance
	(
		glm::vec3(0.15f, 0.2f, -0.3f),
		glm::vec3(0.0f, 190.0f, 0.0f),
		glm::vec3(2.0f),
		&p_sceneGraphRoot.m_worldTrans,
		true,
		nullptr
	);

	m_pistolInstanceHolster = new Instance
	(
		glm::vec3(0.22f, -0.1f, -0.05f),
		glm::vec3(90.0f, 180.0f, 0.0f),
		glm::vec3(1.2f),
		&p_sceneGraphRoot.m_worldTrans,
		false,
		nullptr
	);
	
	m_model->AddInstance(m_playerInstance);
	p_sceneGraphRoot.m_childeren.push_back(dynamic_cast<SceneGraphNode*>(m_playerInstance));
	//lock rotation
	m_playerInstance->rigidBody->m_body->setAngularFactor(btVector3(0, 0, 0));
	m_playerInstance->rigidBody->m_body->setDamping(0.95f, 1.0f); // Linear and angular damping
	m_playerInstance->rigidBody->m_body->setUserPointer(this);

	m_pistol->AddInstance(m_pistolInstanceAiming);
	m_pistol->AddInstance(m_pistolInstanceHolster);
	m_playerInstance->m_childeren.push_back(dynamic_cast<SceneGraphNode*>(m_pistolInstanceHolster));


	m_healthbar = new UIRectangleColor(glm::vec3(0.9f), p_shaderManager.GetShader(ShaderManager::ShaderEnum::uiColorShader), UIElement::Alignment::left);
	m_healthbarPadding = new UIRectangleColor(glm::vec3(0.0f), p_shaderManager.GetShader(ShaderManager::ShaderEnum::uiColorShader), UIElement::Alignment::left);

	m_crosshair = new UITextureRectangle(std::string(ASSETS_DIR) + "ui/crosshair.png", p_shaderManager.GetShader(ShaderManager::ShaderEnum::uiShader));
	m_hitmarker = new UITextureRectangle(std::string(ASSETS_DIR) + "ui/hitMarker.png", p_shaderManager.GetShader(ShaderManager::ShaderEnum::uiShader));
	
	
	glm::mat3 transform = glm::mat3(1.0f); // Start with identity matrix
	transform = Mat3Utils::translate(transform, glm::vec2(-0.95f, 0.5f)); // Apply translation
	transform = Mat3Utils::scale(transform, glm::vec2(0.7f, 0.1f)); // Apply scaling
	m_healthbarPadding->AddInstance(transform);

	transform = glm::mat3(1.0f); // Start with identity matrix
	transform = Mat3Utils::translate(transform, glm::vec2(-0.94f, 0.51f)); // Apply translation
	transform = Mat3Utils::scale(transform, glm::vec2(0.68f, 0.08f)); // Apply scaling
	m_healthbar->AddInstance(transform);

	transform = glm::mat3(1.0f);
	transform = Mat3Utils::scale(transform, glm::vec2(0.2f)); // Apply scaling
	m_crosshair->AddInstance(transform);
	m_hitmarker->AddInstance(transform);


}

Player::~Player()
{
	if (m_pistol != nullptr)
	{
		delete m_pistol;
	}

	if (m_model != nullptr)
	{
		delete m_model;
	}

	if (m_healthbar != nullptr)
	{
		delete m_healthbar;
	}

	if (m_healthbarPadding != nullptr)
	{
		delete m_healthbarPadding;
	}

	if (m_crosshair != nullptr)
	{
		delete m_crosshair;
	}

	if (m_hitmarker != nullptr)
	{
		delete m_hitmarker;
	}
}

void Player::Update(float p_deltatime)
{
	if (m_shootTimer > 0.0f)
	{
		m_shootTimer -= p_deltatime;
	}

	if (m_hitmarkerTimer > 0.0f)
	{
		m_hitmarkerTimer -= p_deltatime;
	}
}

void Player::Render()
{
	if (!m_fpsMode)
	{
		m_model->Draw(m_camera);
		m_pistolInstanceHolster->m_active = true;
		//m_pistolInstanceAiming->m_active = false;

	}
	else
	{
		m_crosshair->Render();
		if (m_hitmarkerTimer > 0.0f)
		{
			m_hitmarker->Render();
		}
		m_pistolInstanceAiming->m_active = true;
		m_pistolInstanceHolster->m_active = false;
	}
	m_pistol->Draw(m_camera);

	m_healthbarPadding->Render();
	m_healthbar->Render();
}


void Player::Move(const glm::vec2& p_dir)
{
	btVector3 surrentVel = m_playerInstance->rigidBody->m_body->getLinearVelocity();
	if ((p_dir.x == 0.0f && p_dir.y == 0.0f) || m_fpsMode)
	{ 
		m_playerInstance->rigidBody->m_body->setLinearVelocity(btVector3(btVector3(0.0f, surrentVel.getY(), 0.0f)));
		return; 
	}

	// Ensure the camera's front vector ignores vertical movement (Y-axis).
	glm::vec3 forward = glm::normalize(glm::vec3(m_camera.front.x, 0.0f, m_camera.front.z));
	glm::vec3 right = glm::normalize(glm::vec3(m_camera.right.x, 0.0f, m_camera.right.z));

	// Combine the forward and right vectors with the input direction.
	glm::vec3 moveDir = right * p_dir.x + forward * p_dir.y;

	// Normalize the movement direction and scale it by the player speed.
	moveDir = glm::normalize(moveDir) * PLAYER_SPEED;
	m_playerInstance->rigidBody->m_body->activate(true);
	
	m_playerInstance->rigidBody->m_body->setLinearVelocity(btVector3(btVector3(moveDir.x, surrentVel.getY(), moveDir.z)));
	
	
	float yaw = atan2(moveDir.x, moveDir.z); // Correct for a flat XZ-plane
	glm::quat newRotation = glm::quat(glm::vec3(0.0f, yaw, 0.0f));

	m_playerInstance->rot = glm::degrees(glm::eulerAngles(newRotation)); // Convert quaternion to Euler angles (in degrees) and update `rot`

}

void Player::Shoot()
{
	if (m_shootTimer <= 0.0f)
	{
		m_shootTimer = SHOOT_COUNTDOWN;
		float rayLength = 30.0f;
		glm::vec3 rayStart = m_camera.position; // Camera position
		glm::vec3 rayEnd = rayStart + glm::normalize(m_camera.front) * rayLength;

		btVector3 btRayStart(rayStart.x, rayStart.y, rayStart.z);
		btVector3 btRayEnd(rayEnd.x, rayEnd.y, rayEnd.z);

		btCollisionWorld::ClosestRayResultCallback rayCallback(btRayStart, btRayEnd);
		rayCallback.m_collisionFilterMask = CollisionManager::CollisionMask::Enemy | CollisionManager::CollisionMask::Environment;
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
				if (objectGroup & CollisionManager::CollisionMask::Enemy)
				{
					Enemy* enemy = static_cast<Enemy*>(hitObject->getUserPointer());
					if (enemy)
					{
						m_hitmarkerTimer = HITMARKER_DUR;
						enemy->TakeDamage(20.0f);
					}
					return;
				}
			}
		}
		else
		{
			m_collisionManager.m_dynamicsWorld->getDebugDrawer()->drawLine(btRayStart, btRayEnd, startColor);
		}

	}
}

void Player::Look(BaseInput::MouseInfo p_mouseInfo)
{
	glm::vec2 newMousePos = glm::vec2(p_mouseInfo.x, p_mouseInfo.y);

	m_fpsMode = ((p_mouseInfo.buttonState & BaseInput::MouseFlag::MOUSE_BTN_R) == BaseInput::MouseFlag::MOUSE_BTN_R);

	// Calculate mouse offset
	glm::vec2 offset = newMousePos - m_lastMousePos;
	offset *= m_mouseSensitivity;

	// Update yaw and pitch angles
	m_camera.yaw += offset.x;
	m_camera.pitch -= offset.y; // Inverted Y-axis for natural behavior

	// Clamp pitch to prevent flipping
	if (m_camera.pitch > 89.0f) m_camera.pitch = 89.0f;
	if (m_camera.pitch < -89.0f) m_camera.pitch = -89.0f;

	// Update last mouse position
	m_lastMousePos = newMousePos;



	if (!m_fpsMode)
	{
		float radius = glm::length(CAM_OFFSET_1); // Distance between player and camera
		float pitchRadians = glm::radians(m_camera.pitch);
		float adjustedYawRadians = glm::radians(m_camera.yaw - 90.0f); // Adjust yaw by 90°

		glm::vec3 cameraOffset;
		cameraOffset.x = radius * cos(pitchRadians) * sin(adjustedYawRadians);	  // X component
		cameraOffset.y = -(radius * sin(pitchRadians));							  // Y component
		cameraOffset.z = -(radius * cos(pitchRadians) * cos(adjustedYawRadians)); // Z component

		// Set the camera's position relative to the player
		m_camera.position = m_playerInstance->pos + cameraOffset;
		// Update the camera's `front` vector to look at the player
		m_camera.front = glm::normalize(m_playerInstance->pos - m_camera.position);
	}
	else
	{
		m_camera.position = m_playerInstance->pos + CAM_OFFSET_2;
		if (((p_mouseInfo.buttonState & BaseInput::MouseFlag::MOUSE_BTN_L) == BaseInput::MouseFlag::MOUSE_BTN_L))
		{
			Shoot();
		}
	}

	// Update the camera's up and right vectors
	m_camera.UpdateCamVecs();
	m_pistolInstanceAiming->m_localTrans = m_camera.GetMatrixOffsetFromCamera(glm::vec3(0.0f, 0.0f, 0.0f));
	

	


}

void Player::TakeDamage(float p_damage)
{
	printf(" player took damage");
	m_health -= p_damage;

	float size = (m_health / MAX_HEALTH) * 0.68f;
	glm::mat3& healthbar = m_healthbar->instances[0];
	healthbar = glm::mat3(1.0f);
	healthbar = Mat3Utils::translate(healthbar, glm::vec2(-0.94f, 0.51f));
	healthbar = Mat3Utils::scale(healthbar, glm::vec2(size, 0.08f));


	if (m_health < 1.0f)
	{
		m_isDead = true;
	}
}
