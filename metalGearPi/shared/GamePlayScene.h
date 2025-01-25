#pragma once

#include "baseScene.h"
#include "sceneGraph.h"

class TextRendererImgui;
class CollisionManager;
class LightingManager;
class ShaderManager;
class RoomManager;
struct Camera;
class Player;
class Game;

class GamePlayScene : public BaseScene
{
	public:
	GamePlayScene(Game& p_game, ShaderManager* p_shaderManager, CollisionManager& p_collisionManager);
	~GamePlayScene();

	bool Init() override;
	void Update(float p_deltaTime) override;
	void UpdateInput(float p_deltaTime) override;
	void Shutdown() override;

	void WinGame();

	private:
	TextRendererImgui* debugDrawerer = nullptr;
	CollisionManager& m_collisionManager;
	ShaderManager& m_shaderManager;
	RoomManager* m_roomManager = nullptr;
	Player* m_player = nullptr;
	Camera* m_cam = nullptr;
	bool m_playerWon = false;

	SceneGraphRoot m_sceneGraphRoot;
	LightingManager* m_lightingManager = nullptr;

};

