#pragma once

#include "UIElement.h"
#include <vector>

#ifdef LINUX_BUILD
#include <glm/glm.hpp>
#endif // LINUX_BUILD

#ifdef WINDOWS_BUILD
#include "glm.hpp"
#endif // WINDOWS_BUILD

#include "mesh.h"
#include "common.h"

class BaseShader;

class UITextureRectangle : public UIElement
{
	public:

	UITextureRectangle(std::string p_texturePath, BaseShader* p_shader, Alignment p_alignment = Alignment::middle);
	~UITextureRectangle();

	void Render() override;
	void AddInstance(glm::mat3 p_instance) { instances.push_back(p_instance); };

	std::vector<glm::mat3> instances;

	protected:
	BaseShader* m_shader;
	uint m_textureID;


	void initializeQuad(Alignment p_alignment = Alignment::middle);

};

class UIRectangleColor : public UIElement
{
	public:
	UIRectangleColor(glm::vec3 p_color, BaseShader* p_shader, Alignment p_alignment = Alignment::middle);
	~UIRectangleColor() {};

	void Render() override;
	void AddInstance(glm::mat3 p_instance) { instances.push_back(p_instance); };

	std::vector<glm::mat3> instances;

	protected:
	BaseShader* m_shader;
	glm::vec3 m_color;

	void initializeQuad(Alignment p_alignment = Alignment::middle);
};