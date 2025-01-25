#pragma once

//#include "baseInput.h"
//#include "baseWindow.h"
//#include "baseShader.h"

class BaseWindow;
class BaseInput;
class ShaderManager;
class TextRenderer;

#include <memory>
#include <iostream>
#include "baseScene.h"

#include "collisionManager.h"
#include "common.h"

//BaseScene

class Game
{
	public:

	enum class Scene
	{
		menu,
		gameplay,
		gameWin,
		gameLose,
	};

	Game(BaseWindow* p_window, BaseInput* p_input, ShaderManager* p_shaderManager);
	void Run();
	void SwitchScene(Scene p_newScene);

	~Game();

	BaseWindow* m_window;
	BaseInput* m_input;
	ShaderManager* m_shaderManager;
	uint m_randomSeed = 0;
	TextRenderer* m_textRenderer = nullptr;


	private:
	std::unique_ptr<BaseScene> m_currentScene = nullptr;
	CollisionManager* m_collisionManager = nullptr;

};

