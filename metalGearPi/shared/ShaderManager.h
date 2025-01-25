#pragma once

#include <vector>
#include "common.h"
#include "baseShader.h"


class ShaderManager
{
	public:
	
	enum class ShaderEnum
	{
		defaultShader,
		uiShader,
		uiColorShader,
		noiseUiShader,
		textShader
	};

	ShaderManager();
	~ShaderManager()
	{
		for (auto& shader : shaders)
		{
			if (shaders != nullptr)
			{
				delete shader;
			}
		}
	}

	void Use(ShaderEnum p_shader);
	BaseShader* GetShader(ShaderEnum p_shader);

	private:
	BaseShader* shaders[5] = {nullptr};

};

