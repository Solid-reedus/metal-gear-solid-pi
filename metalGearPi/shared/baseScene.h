#pragma once

class Game;
class CollisionManager;
class BaseInput;

class BaseScene
{
	public:
	BaseScene(Game& p_game, CollisionManager& p_collisionManager)
		: m_game(p_game), m_collisionManager(p_collisionManager) {}
	virtual ~BaseScene() = default;

	virtual bool Init() = 0;
	virtual void Update(float p_deltaTime) = 0;
	virtual void UpdateInput(float p_deltaTime) = 0;
	virtual void Shutdown() = 0;

	protected:
	Game& m_game;
	CollisionManager& m_collisionManager;



};

