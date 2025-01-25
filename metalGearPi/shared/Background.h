#pragma once

#include "UIRectangle.h"

class BackGround : public UITextureRectangle
{
	public:
	BackGround(std::string p_texturePath, BaseShader* p_shader, BaseShader* p_NoiseShader);
	~BackGround ();

	void Render();
	void Update(float p_deltaTime);

	private:
	uint m_noiseTextureID;
	BaseShader* m_NoiseShader;
	float noiseOffset1 = 0, noiseOffset2 = 0;
	const uint m_backgroundSeed = 4249740705;
};

