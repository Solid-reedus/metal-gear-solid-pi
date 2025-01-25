#include "testScene.h"
#include "collisionManager.h"

#include "debugCameraController.h"
#include "baseInput.h"
#include "game.h"
#include "player.h"
#include "baseImgui.h"
#include "baseWindow.h"
#include "ShaderManager.h"
#include "TriggerManager.h"

#define LINUX_DEBUG // for linux you manualy need to specify if its in debug mode

TestScene::TestScene(Game& p_game, ShaderManager* p_shaderManager, CollisionManager& p_collisionManager)
	: BaseScene(p_game, p_collisionManager), m_shaderManager(p_shaderManager)
{
	const std::vector<std::string> p_text;
	std::vector<std::string> text;
	text.push_back("this is a test");
	text.push_back("second row");
	text.push_back("ignite the FLAME");
	debugDrawerer = new TextRendererImgui(text, 2.0f);
}

bool TestScene::Init()
{
	/*
	glm::vec3 cameraPosition(0.0f, 0.0f, 0.0f);
	glm::vec3 cameraTarget(0.0f, 0.0f, 1.0f);
	m_cam = new Camera(cameraPosition, cameraTarget);

	printf("%f %f %f \n", m_cam->front.x, m_cam->front.y, m_cam->front.z);

	m_monkeys = new ModelInstance(new Model(std::string(ASSETS_DIR) + "monkey/monkey.model"), m_shaderManager->GetShader(ShaderManager::ShaderEnum::defaultShader));
	m_planes = new ModelInstance(new Model(std::string(ASSETS_DIR) + "grassPlane/grassPlane.model"), m_shaderManager->GetShader(ShaderManager::ShaderEnum::defaultShader));

	for (int i = 0; i < 20; i++)
	{
		Instance* ins1 = new Instance
		(
			glm::vec3(1.0f, 6.0f + i, 1.0f),
			glm::vec3(0.0f), 
			glm::vec3(1.0f),
			true,
			new RigidBody(
				*m_collisionManager.m_dynamicsWorld,
				m_collisionManager.GetShape(0),
				btVector3(0.0f, 6.0f, 0.0f), 
				1.0f,
				CollisionManager::CollisionMask::Prop,
				CollisionManager::CollisionMask::All
			)
		);
		m_monkeys->AddInstance(ins1);

	}

	Instance* ins4 = new Instance
	(
		glm::vec3(0.0f, -2.0f, 0.0f),
		glm::vec3(0.0f),
		glm::vec3(40.0f, 0.1f, 40.0f),

		true,
		new RigidBody(
			*m_collisionManager.m_dynamicsWorld,
			m_collisionManager.GetShape(1),
			btVector3(0.0f, -2.0f, 0.0f), 
			0.0f,
			CollisionManager::CollisionMask::Environment,
			CollisionManager::CollisionMask::All
		)

	);

	m_planes->AddInstance(ins4);

	#if defined(_DEBUG) || defined(LINUX_DEBUG)
	debugCam = new DebugCameraController(*m_cam);
	m_game.m_window->LockCursor(true);
	#endif // !_DEBUG
		

	player = new Player(*m_cam, 
		m_shaderManager->GetShader(ShaderManager::ShaderEnum::defaultShader), 
		m_shaderManager->GetShader(ShaderManager::ShaderEnum::uiColorShader), 
		m_collisionManager);
	
	std::function<void()> callBacks = []() { printf("it touched! \n"); };
	m_collisionManager.AddTrigger(0, btVector3(-2.0f, 2.0f, 0.0f), callBacks, CollisionManager::CollisionMask::Trigger, CollisionManager::CollisionMask::Player);
	std::function<void()> callBacks2 = []() { printf("it touched 2! \n"); };
	m_collisionManager.AddTrigger(0, btVector3(-6.0f, 2.0f, 0.0f), callBacks2, CollisionManager::CollisionMask::Trigger, CollisionManager::CollisionMask::Player);
	*/

	return true;

}

void TestScene::Update(float p_deltaTime)
{

	m_planes->Draw(*m_cam);
	m_monkeys->Draw(*m_cam);
	m_collisionManager.Update(p_deltaTime);
	m_collisionManager.Render(*m_cam);

	player->Update(p_deltaTime);
	player->Render();

	debugDrawerer->Draw();

	//printf("cam right %f %f %f \n", m_cam->right.x, m_cam->right.y, m_cam->right.z);
	//printf("cam front %f %f %f \n", m_cam->front.x, m_cam->front.y, m_cam->front.z);

}

void TestScene::Shutdown()
{
	m_collisionManager.ClearWorldObjects();

	//delete m_model;
	delete m_planes;
	delete m_monkeys;
	delete m_cam;

	#if defined(_DEBUG) || defined(LINUX_DEBUG)
	delete debugCam;
	#endif // !_DEBUG

}

void TestScene::UpdateInput(float p_deltaTime)
{

	BaseInput::MouseInfo mouseInfo = m_game.m_input->PollMouse();
	if (m_game.m_input->IsKeyPressed(BaseInput::KeyBoardKey::_KEY_P)) { m_game.m_window->Close(); }

	// remove later
	if (m_game.m_input->IsKeyPressed(BaseInput::KeyBoardKey::_KEY_G)) { m_game.SwitchScene(Game::Scene::menu); return; }

	// player input
	glm::vec2 moveVec = glm::vec2(0.0f);
	if (m_game.m_input->IsKeyPressed(BaseInput::KeyBoardKey::_KEY_W))
		moveVec.y += 1.0f;
	if (m_game.m_input->IsKeyPressed(BaseInput::KeyBoardKey::_KEY_S))
		moveVec.y -= 1.0f;
	if (m_game.m_input->IsKeyPressed(BaseInput::KeyBoardKey::_KEY_A))
		moveVec.x -= 1.0f;
	if (m_game.m_input->IsKeyPressed(BaseInput::KeyBoardKey::_KEY_D))
		moveVec.x += 1.0f;

	// check if the values arent 0.0f otherwise it passes a -nan(ind)
	if (glm::length(moveVec) > 0.0f)
	{
		moveVec = glm::normalize(moveVec);
		player->Move(moveVec);
	}
	else
	{
		player->Move(glm::vec2(0.0f));
	}
	player->Look(mouseInfo);



	#if defined(_DEBUG) || defined(LINUX_DEBUG)
	if (m_game.m_input->IsKeyPressed(BaseInput::KeyBoardKey::_KEY_O))
	{
		if (!debugModeToggled) // Only toggle if it hasn't been handled yet
		{
			m_game.m_window->LockCursor(!debugMode);
			debugMode = !debugMode;
			debugModeToggled = true; // Mark as handled
		}
	}
	else
	{
		debugModeToggled = false; // Reset when the key is released
	}
	if (!debugMode){return;}

	glm::vec2 currentMousePos(mouseInfo.x, mouseInfo.y);
	debugCam->ProcessMouseMovement(currentMousePos, true);
	if (m_game.m_input->IsKeyPressed(BaseInput::KeyBoardKey::_KEY_W))
		debugCam->ProcessKeyboard(DebugCameraController::FORWARD, p_deltaTime);
	if(m_game.m_input->IsKeyPressed(BaseInput::KeyBoardKey::_KEY_S))
		debugCam->ProcessKeyboard(DebugCameraController::BACKWARD, p_deltaTime);
	if (m_game.m_input->IsKeyPressed(BaseInput::KeyBoardKey::_KEY_D))
		debugCam->ProcessKeyboard(DebugCameraController::RIGHT, p_deltaTime);
	if (m_game.m_input->IsKeyPressed(BaseInput::KeyBoardKey::_KEY_A))
		debugCam->ProcessKeyboard(DebugCameraController::LEFT, p_deltaTime);
	if (m_game.m_input->IsKeyPressed(BaseInput::KeyBoardKey::_KEY_E))
		debugCam->ProcessKeyboard(DebugCameraController::UP, p_deltaTime);
	if (m_game.m_input->IsKeyPressed(BaseInput::KeyBoardKey::_KEY_Q))
		debugCam->ProcessKeyboard(DebugCameraController::DOWN, p_deltaTime);

	#endif // !_DEBUG

}

