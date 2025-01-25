#pragma once
#include <string>
#include <iostream>
#include "common.h"

#ifdef LINUX_BUILD
#include "stb/stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#endif // LINUX_BUILD

#ifdef WINDOWS_BUILD
#include "../libraries/windows/stb/stb_image.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#endif // WINDOWS_BUILD





class UIElement
{
	public:

    enum class Alignment
    {
        left,
        middle,
        right
    };

    UIElement()
    {
        VAO = 0;
        VBO = 0;

        ratio = glm::vec2(0.0f);
        ratio.x = 1.0f;
        ratio.y = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;
    }

	virtual ~UIElement() = default;
	
    virtual void Render() = 0;
	//unsigned int createTexture(const std::string& texturePath);
    unsigned int createTexture(const std::string& texturePath)
    {
        unsigned int textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_set_flip_vertically_on_load(true);

        // Load texture data
        int width, height, nrChannels;
        unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cerr << "Failed to load texture: " << texturePath << std::endl;
        }
        stbi_image_free(data);

        stbi_set_flip_vertically_on_load(false);

        glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture
        return textureID;
    }

	protected:
	unsigned int VAO, VBO;

    glm::vec2 ratio;
    //glm::mat4 orthographic;

};