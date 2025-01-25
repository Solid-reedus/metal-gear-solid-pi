#pragma once

#include "baseScene.h"

class Game;
class CollisionManager;
class ShaderManager;
class BackGround;
//class TextRenderer;

class MenuScene : public BaseScene
{
	public:
	MenuScene(Game& p_game, ShaderManager* p_shaderManager, CollisionManager& p_collisionManager);
	~MenuScene();

	bool Init() override;
	void Update(float p_deltaTime) override;
	void UpdateInput(float p_deltaTime) override;
	void Shutdown() override;

	private:

	//BaseShader* m_UIshader, *m_backgroundShader;
	BackGround* m_background = nullptr;
	//TextRenderer* m_textRenderer = nullptr;
};

