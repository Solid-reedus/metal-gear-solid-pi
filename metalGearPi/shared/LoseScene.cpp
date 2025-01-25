#include "LoseScene.h"

#include "common.h"
#include "ShaderManager.h"
#include "TextRenderer.h"

#include "game.h"
#include "baseInput.h"
#include "baseWindow.h"



LoseScene::LoseScene(Game& p_game, ShaderManager*, CollisionManager& p_collisionManager)
	: BaseScene(p_game, p_collisionManager)

{
	m_game.m_window->LockCursor(false);
}

bool LoseScene::Init()
{
	return true;
}

void LoseScene::Update(float)
{
	m_game.m_textRenderer->RenderText("game over", static_cast<int>(SCREEN_WIDTH / 2), static_cast<int>(SCREEN_HEIGHT / 3.5f), 5.0f, glm::vec3(0.8f, 0.2f, 0.2f), TextRenderer::Alignment::Center);
	m_game.m_textRenderer->RenderText("press M to go to menu or press R to try again", static_cast<int>(SCREEN_WIDTH / 2.0f), static_cast<int>(SCREEN_HEIGHT / 2.0f), 1.5f, glm::vec3(1.0), TextRenderer::Alignment::Center);
	m_game.m_textRenderer->RenderText("or press P to exit the game", static_cast<int>(SCREEN_WIDTH / 2), static_cast<int>(SCREEN_HEIGHT / 1.7f), 1.5f, glm::vec3(1.0), TextRenderer::Alignment::Center);
}

void LoseScene::UpdateInput(float)
{
	if (m_game.m_input->IsKeyPressed(BaseInput::KeyBoardKey::_KEY_P)) { m_game.m_window->Close(); return; }
	if (m_game.m_input->IsKeyPressed(BaseInput::KeyBoardKey::_KEY_R)) { m_game.SwitchScene(Game::Scene::gameplay);return; }
	if (m_game.m_input->IsKeyPressed(BaseInput::KeyBoardKey::_KEY_M)) { m_game.SwitchScene(Game::Scene::menu); return;}
}

void LoseScene::Shutdown()
{
}
