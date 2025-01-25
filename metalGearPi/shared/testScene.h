#pragma once

#include "baseScene.h"

//#include "model.h"
#include "camera.h"
#include "common.h"
//#include "glm.hpp"

#include "modelInstance.h"

#include "debugCameraController.h"
#include "player.h"


#define LINUX_DEBUG // for linux you manualy need to specify if its in debug mode

class TextRendererImgui;
class ShaderManager;
class CollisionManager;
class BaseInput;

class TestScene : public BaseScene
{
	public:
	TestScene(Game& p_game, ShaderManager* p_shaderManager, CollisionManager& p_collisionManager);

	bool Init() override;
	void Update(float p_deltaTime) override;
	void Shutdown() override;
	void UpdateInput(float p_deltaTime) override;

	private:
	ModelInstance* m_monkeys = nullptr;
	ModelInstance* m_planes = nullptr;


	#if defined(_DEBUG) || defined(LINUX_DEBUG)
	DebugCameraController* debugCam = nullptr;
	bool debugMode = false;
	bool debugModeToggled = false;
	#endif // _DEBUG

	Camera* m_cam = nullptr;
	ShaderManager* m_shaderManager = nullptr;
	//BaseShader* m_shader = nullptr;
	Player* player = nullptr;

	TextRendererImgui* debugDrawerer;
};

