#include "GamePlayScene.h"

#include "debugCameraController.h"
#include "collisionManager.h"
#include "TriggerManager.h"
#include "ShaderManager.h"
#include "EnemyManager.h"
#include "RoomManager.h"
#include "sceneGraph.h"
#include "baseWindow.h"
#include "baseInput.h"
#include "baseImgui.h"
#include "player.h"
#include "game.h"
#include "lightingManager.h"


GamePlayScene::GamePlayScene(Game& p_game, ShaderManager* p_shaderManager, CollisionManager& p_collisionManager)
	: BaseScene(p_game, p_collisionManager), m_shaderManager(*p_shaderManager), m_collisionManager(p_collisionManager)
{
	m_sceneGraphRoot = SceneGraphRoot();

	std::vector<LightingManager::Sun> suns;
	std::vector<LightingManager::SpotLight> spotLights;
	std::vector<LightingManager::PointLight> pointLights;

	suns.push_back(LightingManager::Sun
	(
		glm::normalize(glm::vec3(-1.0f, -1.0f, -0.5f)),
		glm::vec3(1.0f),
		0.6f
	));

	pointLights.push_back(LightingManager::PointLight
	(
		glm::vec3(1.0f),
		glm::vec3(1.0f),
		1.0f
	));


	m_lightingManager = new LightingManager(suns, spotLights, pointLights);
	BaseShader* defaultSHader = m_shaderManager.GetShader(ShaderManager::ShaderEnum::defaultShader);
	m_lightingManager->SetAsCurrentUBO(defaultSHader);

}

GamePlayScene::~GamePlayScene()
{
	delete m_cam;
	delete m_player;
	delete debugDrawerer;
	delete m_roomManager;
	delete m_lightingManager;
}

bool GamePlayScene::Init()
{

	glm::vec3 cameraPosition(0.0f, 0.0f, 0.0f);
	glm::vec3 cameraTarget(0.0f, 0.0f, 1.0f);
	m_cam = new Camera(cameraPosition, cameraTarget);

	#ifndef NDEBUG
		const std::vector<std::string> p_text;
		std::vector<std::string> text;
		text.push_back("current fps");
		text.push_back("");
		debugDrawerer = new TextRendererImgui(text, 2.0f);
	#endif // !NDEBUG


	m_player = new Player
	(
		*m_cam, 
		m_shaderManager,
		m_collisionManager,
		m_sceneGraphRoot
	);
	
	m_roomManager = new RoomManager
	(
		m_shaderManager.GetShader(ShaderManager::ShaderEnum::defaultShader), 
		*m_cam, 
		m_collisionManager, 
		*m_lightingManager,
		*m_player, 
		m_game.m_randomSeed, 
		m_sceneGraphRoot,
		*this
	);
	
	m_roomManager->LoadNewRoom(0, glm::vec3(0.0f, 8.0f, -6.0f));
	m_game.m_window->LockCursor(true);


	return true;
}

void GamePlayScene::Update(float p_deltaTime)
{
	m_sceneGraphRoot.UpdateMat();

	m_collisionManager.Update(p_deltaTime);
	m_player->Update(p_deltaTime);
	m_roomManager->Update(p_deltaTime);

	BaseShader* defaultSHader = m_shaderManager.GetShader(ShaderManager::ShaderEnum::defaultShader);
	m_lightingManager->SetAsCurrentUBO(defaultSHader);


	m_lightingManager->UpdateLight(
		0, 
		LightingManager::LightType::pointLight, 
		LightingManager::LightAtr::pos, 
		m_player->GetPlayerPosRef() + glm::vec3(0.0f, 2.0f, 0.0f)
	);


	m_roomManager->Render();
	m_player->Render();

	#ifndef NDEBUG
		debugDrawerer->m_text[1] = std::to_string(1 / p_deltaTime);
		debugDrawerer->Draw();
		m_collisionManager.Render(*m_cam);
	#endif // !NDEBUG

	if (m_player->CheckIfPlayerIsDead())
	{
		m_game.SwitchScene(Game::Scene::gameLose);
		return;
	}
	if (m_playerWon)
	{
		m_game.SwitchScene(Game::Scene::gameWin);
		return;
	}
}

void GamePlayScene::UpdateInput(float)
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
		m_player->Move(moveVec);
	}
	else
	{
		m_player->Move(glm::vec2(0.0f));
	}
	m_player->Look(mouseInfo);
}

void GamePlayScene::Shutdown()
{
	m_collisionManager.ClearWorldObjects();
}

void GamePlayScene::WinGame()
{
	printf("player won \n");
	m_playerWon = true;
}

