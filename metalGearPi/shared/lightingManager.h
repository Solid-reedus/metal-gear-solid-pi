#pragma once


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

#include <vector>

#define MAX_SUNS 1
#define MAX_SPOTLIGHTS 5
#define MAX_PointLightS 5

class BaseShader;

class LightingManager
{
	public:

	enum class LightType
	{
		sun,
		spotlight,
		pointLight,
	};

	enum class LightAtr
	{
		pos,
		dir,
		color,
		intensity,
	};

	struct Sun
	{
		Sun(const glm::vec3& p_dir, const glm::vec3& p_color, const float& p_intensity)
			: dir(p_dir), color(p_color), intensity(p_intensity)
		{
		}

		Sun()
		{
			dir = glm::vec3(0.0f);
			color = glm::vec3(0.0f);
			intensity = 0.0f;
		}

		glm::vec3 dir;       // 12 bytes for data, aligned to 16 bytes
		glm::vec3 color;     // 12 bytes for data, aligned to 16 bytes
		float intensity;     // 4 bytes
	};

	struct SpotLight
	{
		SpotLight(const glm::vec3& p_pos, const glm::vec3& p_dir, const glm::vec3& p_color, const float& p_intensity)
			: pos(p_pos), dir(p_dir), color(p_color), intensity(p_intensity)
		{
		}

		SpotLight()
		{
			pos = glm::vec3(0.0f);
			dir = glm::vec3(0.0f);
			color = glm::vec3(0.0f);
			intensity = 0.0f;
		}

		glm::vec3 pos;        // 12 bytes
		glm::vec3 dir;        // 12 bytes
		glm::vec3 color;      // 12 bytes
		float intensity;      // 4 bytes

	};

	struct PointLight
	{
		PointLight(const glm::vec3& p_pos, const glm::vec3& p_color, const float& p_intensity)
			: pos(p_pos), color(p_color), intensity(p_intensity)
		{
		}

		PointLight()
		{
			pos = glm::vec3(0.0f);
			color = glm::vec3(0.0f);
			intensity = 0.0f;
		}

		glm::vec3 pos;
		glm::vec3 color;

		float intensity;
	};

	LightingManager();
	// use this constructor preferably since setting all lights at once is less work
	LightingManager(const std::vector<Sun>& p_suns, const std::vector<SpotLight>& p_spotlights, const std::vector<PointLight>& p_PointLights);

	void AddSun(Sun p_sun);
	void AddSpotLight(SpotLight p_spotLight);
	void AddPointLight(PointLight p_PointLight);

	void SubtractSun(int p_index);
	void SubtractSpotLight(int p_index);
	void SubtractPointLight(int p_index);

	void ClearSuns();
	void ClearSpotLights();
	void ClearPointLights();


	const Sun* LookAtSun(int p_index) const;
	const SpotLight* LookAtSpotLight(int p_index) const;
	const PointLight* LookAtPointLight(int p_index) const;

	// update speseifck light
	void UpdateLight(int p_index, LightType p_lightType, LightAtr p_lightAtr, glm::vec3 p_val);
	//void UpdateLight(int p_index, LightType p_lightType, LightAtr p_lightAtr, float p_val);



	const unsigned int* LookAtLightingUBO() const { return &lightingUBO; };
	void SetAsCurrentUBO(BaseShader* p_usedShader);

private:

	static constexpr int SUN_BUFFER_SIZE = 32;
	static constexpr int SPOT_LIGHT_BUFFER_SIZE = 48;
	static constexpr int POINT_LIGHT_BUFFER_SIZE = 32;

	static constexpr int SPOT_LIGHT_BUFFER_START = MAX_SUNS * SUN_BUFFER_SIZE;
	static constexpr int POINT_LIGHT_BUFFER_START = MAX_SUNS * SUN_BUFFER_SIZE + MAX_SPOTLIGHTS * SPOT_LIGHT_BUFFER_SIZE;
	static constexpr int LIGHTS_SIZE_BUFFER_START = MAX_SUNS * SUN_BUFFER_SIZE + MAX_SPOTLIGHTS * SPOT_LIGHT_BUFFER_SIZE + MAX_PointLightS * POINT_LIGHT_BUFFER_SIZE;

	void SetSubBufferVal(int p_offset, const glm::vec3& p_val);
	void SetSubBufferVal(int p_offset, const float& p_val);
	void InitUBO();
	void UpdateUBO();

	unsigned int lightingUBO;

	std::vector<Sun> m_suns;
	std::vector<SpotLight> m_spotlights;
	std::vector<PointLight> m_pointLights;

};

