#include "TextRenderer.h"

#include <iostream>

#ifdef LINUX_BUILD
#include "stb/stb_image.h"
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include "../gameLinux/lnx_shader.h"
#endif // LINUX_BUILD

#ifdef WINDOWS_BUILD
#include "../libraries/windows/stb/stb_image.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../gameWindows/win_shader.h"
#endif // WINDOWS_BUILD

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "baseShader.h"
#include "camera.h"

#include <ft2build.h>
#include FT_FREETYPE_H


#include "common.h"

TextRenderer::TextRenderer(unsigned int width, unsigned int height, BaseShader* p_shader)
    : m_shader(p_shader)
{
    m_shader->use();
    m_shader->setVec3("textColor", glm::vec3(1.0f));
    m_shader->setMat4("projection", glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f));

    // configure VAO/VBO for texture quads
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


void TextRenderer::Load(std::string font, unsigned int fontSize)
{
    // first clear the previously loaded Characters
    m_characters.clear();
    // then initialize and load the FreeType library
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
    }
    FT_Face face;
    if (FT_New_Face(ft, font.c_str(), 0, &face))
    {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
    }
    // set size to load glyphs as
    FT_Set_Pixel_Sizes(face, 0, fontSize);
    // disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    // then for the first 128 ASCII characters, pre-load/compile their characters and store them
    for (GLubyte c = 0; c < 128; c++) // lol see what I did there 
    {
        // load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // now store character for later use
        Character character = 
        {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<unsigned int>(face->glyph->advance.x)
        };
        m_characters.insert(std::pair<char, Character>(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    // destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

void TextRenderer::RenderText(std::string text, float x, float y, float scale, glm::vec3 color, Alignment p_alignment)
{
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_shader->use();
    m_shader->setVec3("textColor", color);
    m_shader->setMat4("projection", glm::ortho(0.0f, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT, 0.0f));

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(this->VAO);

    // Calculate text width
    float textWidth = 0.0f;
    for (char c : text)
    {
        Character ch = m_characters[c];
        textWidth += static_cast<float>(ch.Advance >> 6) * scale;
    }

    // Adjust x based on alignment
    if (p_alignment == Alignment::Center)
    {
        x -= textWidth / 2.0f;
    }
    else if (p_alignment == Alignment::Right)
    {
        x -= textWidth;
    }

    // Rendering loop
    for (char c : text)
    {
        Character ch = m_characters[c];

        float xpos = x + static_cast<float>(ch.Bearing.x) * scale;
        float ypos = y + static_cast<float>(m_characters['H'].Bearing.y - ch.Bearing.y) * scale;

        float w = static_cast<float>(ch.Size.x) * scale;
        float h = static_cast<float>(ch.Size.y) * scale;

        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 0.0f },
            { xpos,     ypos,       0.0f, 0.0f },

            { xpos,     ypos + h,   0.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 0.0f }
        };

        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Advance cursor for the next glyph
        x += static_cast<float>(ch.Advance >> 6) * scale;
    }


    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}
