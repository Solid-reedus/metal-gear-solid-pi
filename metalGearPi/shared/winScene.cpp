#include "winScene.h"

#include "common.h"
#include "ShaderManager.h"
#include "TextRenderer.h"

#include "game.h"
#include "baseInput.h"
#include "baseWindow.h"


WinScene::WinScene(Game& p_game, ShaderManager*, CollisionManager& p_collisionManager)
	: BaseScene(p_game, p_collisionManager)

{
	m_game.m_window->LockCursor(false);
}

bool WinScene::Init()
{
	return true;
}

void WinScene::Update(float)
{
	m_game.m_textRenderer->RenderText("you won", static_cast<int>(SCREEN_WIDTH / 2), 100, 5.0f, glm::vec3(0.2f, 0.8f, 0.2f), TextRenderer::Alignment::Center);
	m_game.m_textRenderer->RenderText("press M to go to menu or press R to try again", static_cast<int>(SCREEN_WIDTH / 2), 300, 1.5f, glm::vec3(1.0), TextRenderer::Alignment::Center);
	m_game.m_textRenderer->RenderText("or press P to exit the game", static_cast<int>(SCREEN_WIDTH / 2), static_cast<int>(SCREEN_HEIGHT / 1.7f), 1.5f, glm::vec3(1.0), TextRenderer::Alignment::Center);

}

void WinScene::UpdateInput(float)
{
	if (m_game.m_input->IsKeyPressed(BaseInput::KeyBoardKey::_KEY_P)) { m_game.m_window->Close(); return; }
	if (m_game.m_input->IsKeyPressed(BaseInput::KeyBoardKey::_KEY_R)) { m_game.SwitchScene(Game::Scene::gameplay); return; }
	if (m_game.m_input->IsKeyPressed(BaseInput::KeyBoardKey::_KEY_M)) { m_game.SwitchScene(Game::Scene::menu); return; }
}


void WinScene::Shutdown()
{
}
