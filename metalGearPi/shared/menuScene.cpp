#include "menuScene.h"
#include "Background.h"
#include "common.h"
#include "ShaderManager.h"
#include "TextRenderer.h"

#include "game.h"
#include "baseInput.h"
#include "baseWindow.h"

#include "UIRectangle.h"
#include "mat3Functions.h"


MenuScene::MenuScene(Game& p_game, ShaderManager* p_shaderManager, CollisionManager& p_collisionManager)
	: BaseScene(p_game, p_collisionManager)
{
	m_background = new BackGround(
		std::string(ASSETS_DIR) + "grassPlane/diffuse.png", 
		p_shaderManager->GetShader(ShaderManager::ShaderEnum::uiShader),
		p_shaderManager->GetShader(ShaderManager::ShaderEnum::noiseUiShader)
	);

	//glm::mat3(1.0f)
	m_game.m_window->LockCursor(false);

}

MenuScene::~MenuScene()
{
	delete m_background;
}

bool MenuScene::Init()
{

	return true;
}

void MenuScene::Update(float p_deltaTime)
{
	m_background->Update(p_deltaTime);
	m_background->Render();
	m_game.m_textRenderer->RenderText("metal gear solid", static_cast<int>(SCREEN_WIDTH / 2), static_cast<int>(SCREEN_HEIGHT / 3.5f), 4.0f, glm::vec3(1.0f), TextRenderer::Alignment::Center);
	m_game.m_textRenderer->RenderText(std::string(GAME_VERSION) + " edition", static_cast<int>(SCREEN_WIDTH / 2), static_cast<int>(SCREEN_HEIGHT / 2.2f), 2.5f, glm::vec3(1.0f), TextRenderer::Alignment::Center);
	m_game.m_textRenderer->RenderText("press S to start or P to quit game", static_cast<int>(SCREEN_WIDTH / 2), static_cast<int>(SCREEN_HEIGHT / 1.7f), 1.5f, glm::vec3(1.0f), TextRenderer::Alignment::Center);

}

void MenuScene::UpdateInput(float)
{
	if (m_game.m_input->IsKeyPressed(BaseInput::KeyBoardKey::_KEY_P)) { m_game.m_window->Close(); }
	if (m_game.m_input->IsKeyPressed(BaseInput::KeyBoardKey::_KEY_S)) { m_game.SwitchScene(Game::Scene::gameplay); }

}

void MenuScene::Shutdown()
{
}
