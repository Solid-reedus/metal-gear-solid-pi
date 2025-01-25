#pragma once

#include "baseRenderable.h"

class BaseUpdatable  : public BaseRenderable
{
	public:
	BaseUpdatable(Camera& p_cameraRef) : BaseRenderable(p_cameraRef) {}
	virtual void Update(float p_deltatime) = 0;
	virtual ~BaseUpdatable() = default;

	protected:
	
};
