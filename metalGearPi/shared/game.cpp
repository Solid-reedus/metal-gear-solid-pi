#include "game.h"

#ifdef LINUX_BUILD
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#endif // LINUX_BUILD

#ifdef WINDOWS_BUILD
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#endif // WINDOWS_BUILD

#include "baseInput.h"
#include "baseWindow.h"

#include "collisionManager.h"
#include "ShaderManager.h"
#include "TextRenderer.h"
#include "AllScenes.h"
#include <iostream>
#include <memory>
#include <string>
#include <chrono>


Game::Game(BaseWindow* p_window, BaseInput* p_input, ShaderManager* p_shaderManager)
	: m_window(p_window), m_input(p_input), m_shaderManager(p_shaderManager)
{
	m_collisionManager = new CollisionManager();
	m_randomSeed = static_cast<uint>(time(0));

	m_textRenderer = new TextRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, p_shaderManager->GetShader(ShaderManager::ShaderEnum::textShader));
	m_textRenderer->Load(std::string(ASSETS_DIR) + "MGS2.ttf", FONT_SIZE);
}

void Game::Run()
{
	m_currentScene = std::unique_ptr<MenuScene>(new MenuScene(*this, m_shaderManager, *m_collisionManager));
	//m_currentScene = std::unique_ptr<WinScene>(new WinScene(*this, m_shaderManager, *m_collisionManager));
	
	//m_currentScene = std::unique_ptr<TestScene>(new TestScene(*this, m_shaderManager, *m_collisionManager));
	//m_currentScene = std::make_unique<TestScene>(TestScene(*this, m_shaderManager, *m_collisionManager));

	m_currentScene->Init();

	float deltaTime = 0;
	auto startTime = std::chrono::system_clock::now();
	auto lastTime = startTime;

	while (!m_window->ShouldClose())
	{
		m_window->Clear();
		//m_shader->use();

		auto time = std::chrono::system_clock::now();
		std::chrono::duration<float> delta = time - lastTime;
		deltaTime = delta.count();

		//printf("deltaTime %f \n", 1 / deltaTime);

		m_currentScene->UpdateInput(deltaTime);
		m_currentScene->Update(deltaTime);
		m_window->Update();

		lastTime = time;

	}
	m_currentScene->Shutdown();

}

void Game::SwitchScene(Scene p_newScene)
{
	// set screen to loading
	m_window->Clear();
	m_textRenderer->RenderText("loading...", 30, SCREEN_HEIGHT - (FONT_SIZE * 4), 3.0f, glm::vec3(1.0f));
	m_window->Update();

	switch (p_newScene)
	{
		case Game::Scene::menu:
		{
			m_currentScene->Shutdown();
			m_currentScene = std::unique_ptr<MenuScene>(new MenuScene(*this, m_shaderManager, *m_collisionManager));
			m_currentScene->Init();
			return;
		}
		case Game::Scene::gameplay:
		{
			m_currentScene->Shutdown();
			m_currentScene = std::unique_ptr<GamePlayScene>(new GamePlayScene(*this, m_shaderManager, *m_collisionManager));
			m_currentScene->Init();
			return;
		}
		case Game::Scene::gameWin:
		{
			m_currentScene->Shutdown();
			m_currentScene = std::unique_ptr<WinScene>(new WinScene(*this, m_shaderManager, *m_collisionManager));
			m_currentScene->Init();
			return;
		}
		case Game::Scene::gameLose:
		{
			m_currentScene->Shutdown();
			m_currentScene = std::unique_ptr<LoseScene>(new LoseScene(*this, m_shaderManager, *m_collisionManager));
			m_currentScene->Init();
			return;
		}
		default:
		break;
	}
}

Game::~Game()
{
	delete m_window;
	delete m_input;
	delete m_shaderManager;

	delete m_collisionManager;
	delete m_textRenderer;
}