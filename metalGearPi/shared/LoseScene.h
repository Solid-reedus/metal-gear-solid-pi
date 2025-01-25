#pragma once

#include "baseScene.h"

class Game;
class CollisionManager;
class ShaderManager;

class LoseScene : public BaseScene
{
public:
	LoseScene(Game& p_game, ShaderManager* p_shaderManager, CollisionManager& p_collisionManager);

	bool Init() override;
	void Update(float p_deltaTime) override;
	void UpdateInput(float p_deltaTime) override;
	void Shutdown() override;

private:


};

