#pragma once

#include "baseScene.h"

class Game;
class CollisionManager;
class ShaderManager;

class WinScene : public BaseScene
{
	public:
	WinScene(Game& p_game, ShaderManager* p_shaderManager, CollisionManager& p_collisionManager);

	bool Init() override;
	void Update(float p_deltaTime) override;
	void UpdateInput(float p_deltaTime) override;
	void Shutdown() override;

	private:


};

