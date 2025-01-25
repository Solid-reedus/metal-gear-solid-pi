#pragma once

#include <map>
//#include <glm/glm.hpp>
#include <iostream>
#include <vector>



#ifdef LINUX_BUILD
#include <glm/glm.hpp>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#endif // LINUX_BUILD

#ifdef WINDOWS_BUILD
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#endif // WINDOWS_BUILD

class BaseShader;

class TextRenderer
{
    public:

    struct Character
    {
        unsigned int TextureID;
        glm::ivec2   Size;     
        glm::ivec2   Bearing;  
        unsigned int Advance;  
    };


    enum class Alignment 
    {
        Left,
        Center,
        Right
    };

    TextRenderer(unsigned int width, unsigned int height, BaseShader* p_shader);


    void RenderText(std::string text, float x, float y, float scale, glm::vec3 color = glm::vec3(1.0f), Alignment p_alignment = Alignment::Left);
    void Load(std::string font, unsigned int fontSize);
    private:

    std::map<char, Character> m_characters;
    BaseShader* m_shader;
    // render state
    unsigned int VAO = 0, VBO = 0;


};