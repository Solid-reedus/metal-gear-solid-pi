#include "lightingManager.h"


#ifdef LINUX_BUILD
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#endif // LINUX_BUILD

#ifdef WINDOWS_BUILD
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#endif // WINDOWS_BUILD

#include "baseShader.h"
#include <iostream>


LightingManager::LightingManager()
	: m_suns(), m_spotlights(), m_pointLights(), lightingUBO()
{
	InitUBO();
	UpdateUBO();
}

LightingManager::LightingManager(const std::vector<Sun>& p_suns, const std::vector<SpotLight>& p_spotlights, const std::vector<PointLight>& p_PointLights)
	: m_suns(p_suns), m_spotlights(p_spotlights), m_pointLights(p_PointLights), lightingUBO()
{
	InitUBO();
	UpdateUBO();
}

void LightingManager::AddSun(Sun p_sun) { m_suns.push_back(p_sun); UpdateUBO();}
void LightingManager::AddSpotLight(SpotLight p_spotLight) { m_spotlights.push_back(p_spotLight); UpdateUBO(); }
void LightingManager::AddPointLight(PointLight p_PointLight) { m_pointLights.push_back(p_PointLight); UpdateUBO();}


void LightingManager::SubtractSun(int p_index)
{
	if (p_index < 0 || p_index >= m_suns.size())
	{
		printf("error: p_index in LightingManager::SubtractSun is out of bounds \n");
		return;
	}
	m_suns.erase(m_suns.begin() + p_index);
	UpdateUBO();
}

void LightingManager::SubtractSpotLight(int p_index)
{
	if (p_index < 0 || p_index >= m_spotlights.size())
	{
		printf("error: p_index in LightingManager::SubtractSpotLight is out of bounds \n");
		return;
	}
	m_spotlights.erase(m_spotlights.begin() + p_index);
	UpdateUBO();
}

void LightingManager::SubtractPointLight(int p_index)
{
	if (p_index < 0 || p_index >= m_pointLights.size())
	{
		printf("error: p_index in LightingManager::SubtractPointLight is out of bounds \n");
		return;
	}
	m_pointLights.erase(m_pointLights.begin() + p_index);
	UpdateUBO();
}

void LightingManager::ClearSuns() 
{
	m_suns.clear(); 
	UpdateUBO();
}

void LightingManager::ClearSpotLights() 
{
	m_spotlights.clear();
	UpdateUBO();
}
void LightingManager::ClearPointLights() 
{
	m_pointLights.clear();
}

const LightingManager::Sun* LightingManager::LookAtSun(int p_index) const
{
	if (p_index < 0 || p_index >= m_suns.size())
	{
		printf("error: p_index in LightingManager::LookAtSun is out of bounds \n");
		return nullptr;
	}
	return &m_suns[p_index];
}

const LightingManager::SpotLight* LightingManager::LookAtSpotLight(int p_index) const
{
	if (p_index < 0 || p_index >= m_spotlights.size())
	{
		printf("error: p_index in LightingManager::SubtractSpotLight is out of bounds \n");
		return nullptr;
	}
	return &m_spotlights[p_index];
}

const LightingManager::PointLight* LightingManager::LookAtPointLight(int p_index) const
{
	if (p_index < 0 || p_index >= m_pointLights.size())
	{
		printf("error: p_index in LightingManager::SubtractPointLight is out of bounds \n");
		return nullptr;
	}
	return &m_pointLights[p_index];
}

void LightingManager::UpdateLight(int p_index, LightType p_lightType, LightAtr p_lightAtr, glm::vec3 p_val)
{
	// sun starts at 0
	int offset = 0;

	switch (p_lightType)
	{
		case LightingManager::LightType::sun:
		{
			if (p_index >= m_suns.size())
			{
				printf("error: p_index is out of bounds of sun size in LightingManager::UpdateLight \n");
				return;
			}

			switch (p_lightAtr)
			{
				case LightingManager::LightAtr::pos:
				{
					printf("error: sun doesnt have a attribute pos, in LightingManager::UpdateLight \n");
					return;
				}
				case LightingManager::LightAtr::dir:
				{
					offset += p_index * SUN_BUFFER_SIZE;
					SetSubBufferVal(offset, p_val);
					return;
				}
				case LightingManager::LightAtr::color:
				{
					offset += p_index * SUN_BUFFER_SIZE + 16;
					SetSubBufferVal(offset, p_val);
					return;
				}
				case LightingManager::LightAtr::intensity:
				{
					return;
				}
				default:
				return;
			}
			break;
		}
		case LightingManager::LightType::spotlight:
		{
			if (p_index >= m_spotlights.size())
			{
				return;
			}

			offset = SPOT_LIGHT_BUFFER_START;

			switch (p_lightAtr)
			{
				case LightingManager::LightAtr::pos:
				{
					offset += p_index * SPOT_LIGHT_BUFFER_SIZE;
					SetSubBufferVal(offset, p_val);
					return;
				}
				case LightingManager::LightAtr::dir:
				{
					offset += p_index * SPOT_LIGHT_BUFFER_SIZE + 16;
					SetSubBufferVal(offset, p_val);
					return;
				}
				case LightingManager::LightAtr::color:
				{
					offset += p_index * SPOT_LIGHT_BUFFER_SIZE + (16 * 2);
					SetSubBufferVal(offset, p_val);
					return;
				}
				case LightingManager::LightAtr::intensity:
				{
					offset += p_index * SPOT_LIGHT_BUFFER_SIZE + (16 * 2) + 12;
					SetSubBufferVal(offset, p_val.x);

					return;
				}
				default:
				break;
			}

			break;
		}
		case LightingManager::LightType::pointLight:
		{
			if (p_index >= m_pointLights.size())
			{
				return;
			}

			offset = POINT_LIGHT_BUFFER_START;

			switch (p_lightAtr)
			{
				case LightingManager::LightAtr::pos:
				{
					offset += p_index * POINT_LIGHT_BUFFER_SIZE;
					SetSubBufferVal(offset, p_val);
					return;
				}
				case LightingManager::LightAtr::dir:
				{
					printf("error: PointLight doesnt have a attribute dir, in LightingManager::UpdateLight \n");
					return;
				}
				case LightingManager::LightAtr::color:
				{
					offset += p_index * POINT_LIGHT_BUFFER_SIZE + 16;
					SetSubBufferVal(offset, p_val);
					return;
				}
				case LightingManager::LightAtr::intensity:
				{
					return;
				}
				default:
				break;
			}


			break;
		}
		default:
		printf("error:type in LightingManager::UpdateLight is invalid \n");
		break;
	}
}

void LightingManager::SetAsCurrentUBO(BaseShader* p_usedShader)
{
	//p_usedShader->use();  // Bind the shader

	unsigned int lightingBlockIndex = glGetUniformBlockIndex(p_usedShader->ID, "Lighting");
	if (lightingBlockIndex != GL_INVALID_INDEX)
	{
		glUniformBlockBinding(p_usedShader->ID, lightingBlockIndex, 0);
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, lightingUBO);
	}
	else
	{
		std::cerr << "UBO block 'Lighting' not found in shader." << std::endl;
	}
}

void LightingManager::SetSubBufferVal(int p_offset, const glm::vec3& p_val)
{
	glBindBuffer(GL_UNIFORM_BUFFER, lightingUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, p_offset, sizeof(glm::vec3), &p_val);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void LightingManager::SetSubBufferVal(int p_offset, const float& p_val)
{
	glBindBuffer(GL_UNIFORM_BUFFER, lightingUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, p_offset, sizeof(float), &p_val);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void LightingManager::InitUBO()
{
	int maxSize = LIGHTS_SIZE_BUFFER_START + 16; // For the 3 padded ints (numSuns, numSpotlights, numPointLights) and padding_

	glGenBuffers(1, &lightingUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, lightingUBO);
	glBufferData(GL_UNIFORM_BUFFER, maxSize, NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, lightingUBO);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, lightingUBO, 0, maxSize);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void LightingManager::UpdateUBO()
{
	glBindBuffer(GL_UNIFORM_BUFFER, lightingUBO);

	// Calculate the total size for the UBO
	//int uboSize = LIGHTS_SIZE_BUFFER_START + 16; // 3 integers (numSuns, numSpotlights, numPointLights) each aligned to 16 bytes

	// Allocate buffer space
	//glBufferData(GL_UNIFORM_BUFFER, uboSize, nullptr, GL_STATIC_DRAW);

	int offset = 0;

	// Upload `Sun` data
	for (int i = 0; i < m_suns.size(); i++)
	{
		glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(glm::vec3), &m_suns[i].dir);
		offset += 16;

		glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(glm::vec3), &m_suns[i].color);
		offset += 12;

		glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(float), &m_suns[i].intensity);
		offset += 4;
	}

	// 96
	// Move offset for the next block of lights
	offset = SPOT_LIGHT_BUFFER_START;
	// Upload `SpotLight` data
	for (int i = 0; i < m_spotlights.size(); i++)
	{
		offset = SPOT_LIGHT_BUFFER_START + (i * 48);
		glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(glm::vec3), &m_spotlights[i].pos);
		offset += 16;

		glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(glm::vec3), &m_spotlights[i].dir);
		offset += 16;

		glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(glm::vec3), &m_spotlights[i].color);
		offset += 12;

		glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(float), &m_spotlights[i].intensity);
		offset += 4;
	}

	// Move offset for the next block of lights
	offset = POINT_LIGHT_BUFFER_START;

	// Upload `PointLight` data
	for (int i = 0; i < m_pointLights.size(); i++)
	{
		glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(glm::vec3), &m_pointLights[i].pos);
		offset += 16;

		glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(glm::vec3), &m_pointLights[i].color);
		offset += 12;

		glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(float), &m_pointLights[i].intensity);
		offset += 4;
	}

	offset = LIGHTS_SIZE_BUFFER_START;

	float sunSize = static_cast<float>(m_suns.size());
	float spotLightSize = static_cast<float>(m_spotlights.size());
	float PointLightSize = static_cast<float>(m_pointLights.size());

	glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(int), &sunSize);
	glBufferSubData(GL_UNIFORM_BUFFER, offset + 4, sizeof(int), &spotLightSize);
	glBufferSubData(GL_UNIFORM_BUFFER, offset + 8, sizeof(int), &PointLightSize);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
