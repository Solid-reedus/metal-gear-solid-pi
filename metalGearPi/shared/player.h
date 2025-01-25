#pragma once

#include "baseUpdatable.h"
#include "modelInstance.h"
#include "baseInput.h"

struct Camera;
class ShaderManager;
class CollisionManager;
class UIRectangleColor;
class UITextureRectangle;

class Player : public BaseUpdatable
{
	public:
	Player(Camera& p_camera, ShaderManager& p_shaderManager, CollisionManager& p_collisionManager, SceneGraphRoot& p_sceneGraphRoot);
	~Player();

	void Update(float p_deltatime) override;
	void Render() override;
	void Move(const glm::vec2& p_dir);
	void Shoot();
	void Look(BaseInput::MouseInfo p_mouseInfo);
	void TakeDamage(float p_damage);

	void SetPos(const glm::vec3 p_newPos) { m_playerInstance->SetPos(p_newPos);};
	const glm::vec3& GetPlayerPosRef() { return m_playerInstance->pos; }
	const bool CheckIfPlayerIsDead() { return m_isDead; }

	private:
	const float MAX_HEALTH = 100.0f;
	const float PLAYER_SPEED = 15.0f;
	const float SHOOT_COUNTDOWN = 1.0f;
	const float HITMARKER_DUR = 0.3f;
	const glm::vec3 CAM_OFFSET_1 = glm::vec3(0.0f, 7.0f, -12.0f);
	const glm::vec3 CAM_OFFSET_2 = glm::vec3(0.0f, 2.2f, 0.0f);


	ModelInstance* m_model = nullptr;
	ModelInstance* m_pistol = nullptr;
	
	Camera& m_camera;
	CollisionManager& m_collisionManager;
	Instance* m_playerInstance = nullptr;

	Instance* m_pistolInstanceHolster = nullptr;
	Instance* m_pistolInstanceAiming = nullptr;

	UIRectangleColor* m_healthbar = nullptr;
	UIRectangleColor* m_healthbarPadding = nullptr;

	UITextureRectangle* m_crosshair = nullptr;
	UITextureRectangle* m_hitmarker = nullptr;


	glm::vec2 m_lastMousePos = glm::vec2(0, 0);
	float m_mouseSensitivity = 0.07f;
	bool m_fpsMode = false;
	float m_shootTimer = 0.0f;
	float m_hitmarkerTimer = 0.0f;

	float m_health = MAX_HEALTH;
	bool m_isDead = false;



};

