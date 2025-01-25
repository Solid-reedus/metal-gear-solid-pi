#include "UIRectangle.h"

UITextureRectangle::UITextureRectangle(std::string p_texturePath, BaseShader* p_shader, Alignment p_alignment) : m_shader(p_shader)
{
    m_textureID = createTexture(p_texturePath);
    initializeQuad(p_alignment);
}

UITextureRectangle::~UITextureRectangle()
{
    glDeleteTextures(1, &m_textureID);
}

void UITextureRectangle::Render()
{
    // Disable depth testing
    glDisable(GL_DEPTH_TEST);

    // Enable blending and set the blending function
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Use the shader program
    m_shader->use();
    m_shader->setVec2("screenRatio", ratio);

    // Bind the texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    m_shader->setInt("aTexture", 0);

    // Bind VAO and render each instance
    glBindVertexArray(VAO);
    for (const auto& ins : instances)
    {
        m_shader->setMat3("model", ins); // Pass model matrix to shader
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4); // Render the quad
    }

    // Unbind VAO and texture
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Restore OpenGL state
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}



void UITextureRectangle::initializeQuad(Alignment p_alignment)
{
    float vertices[16] =
    {
        // Position         // Texture coordinates
       -0.5f, -0.5f,       0.0f, 0.0f, // Bottom-left
        0.5f, -0.5f,       1.0f, 0.0f, // Bottom-right
        0.5f,  0.5f,       1.0f, 1.0f, // Top-right
       -0.5f,  0.5f,       0.0f, 1.0f  // Top-left
    };

    switch (p_alignment)
    {
        case Alignment::left:
        {
            // Align the quad to the left
            for (int i = 0; i < 16; i += 4)
            {
                vertices[i] += 0.5f; // Shift x-coordinates right by 0.5
            }
            break;
        }
        case Alignment::right:
        {
            // Align the quad to the right
            for (int i = 0; i < 16; i += 4)
            {
                vertices[i] -= 0.5f; // Shift x-coordinates left by 0.5
            }
            break;
        }
        case UITextureRectangle::Alignment::middle: // No change needed; the quad is already centered
        default:
        break;
    }

    // Generate VAO and VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // Bind VAO and VBO
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Upload vertex data to GPU
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Set position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0); // First 2 floats are position
    glEnableVertexAttribArray(0);

    // Set texture coordinate attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float))); // Next 2 floats are texture coordinates
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0); // Unbind VAO
}

UIRectangleColor::UIRectangleColor(glm::vec3 p_color, BaseShader* p_shader, Alignment p_alignment)
    : m_shader(p_shader), m_color(p_color)
{
    initializeQuad(p_alignment);
}

void UIRectangleColor::initializeQuad(Alignment p_alignment)
{
    float vertices[8] = 
    {
        // Position 
       -0.5f, -0.5f,
        0.5f, -0.5f,
        0.5f,  0.5f,
       -0.5f,  0.5f
    };

    // Modify vertices for alignment
    switch (p_alignment)
    {
        case Alignment::left:
        {
            for (int i = 0; i < 8; i++)
            {
                vertices[i] += 0.5f; // Shift x-coordinates right
            }
            break;
        }

        case Alignment::right:
        {
            for (int i = 0; i < 8; i++)
            {
                vertices[i] -= 0.5f; // Shift x-coordinates left
            }
            break;

        }

        case Alignment::middle:
        default:
        break;
    }

    // Generate VAO and VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Upload vertex data
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Configure position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0); // Position only
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0); // Unbind VAO
}


void UIRectangleColor::Render()
{
    glDisable(GL_DEPTH_TEST); // UI elements are usually rendered in 2D

    m_shader->use();
    m_shader->setVec3("color", m_color); // Set color uniform
    m_shader->setVec2("screenRatio", ratio);

    glBindVertexArray(VAO);

    for (const auto& ins : instances)
    {
        m_shader->setMat3("model", ins); // Pass model matrix
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4); // Draw quad
    }

    glBindVertexArray(0);
    glEnable(GL_DEPTH_TEST); // Re-enable depth testing after rendering UI
}