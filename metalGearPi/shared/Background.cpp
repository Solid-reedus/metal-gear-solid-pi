#include "Background.h"
#include "Random.h"

BackGround::BackGround(std::string p_texturePath, BaseShader* p_shader, BaseShader* p_NoiseShader)
	: UITextureRectangle(p_texturePath, p_shader), m_NoiseShader(p_NoiseShader)
{
	m_noiseTextureID = createNoiseTexture(512, m_backgroundSeed, 45.0f);
}

BackGround::~BackGround()
{
    glDeleteTextures(1, &m_noiseTextureID);
}

void BackGround::Render()
{
    UITextureRectangle::Render();

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_NoiseShader->use();
    m_NoiseShader->setMat3("model", glm::mat3(2.0f));
    //m_NoiseShader->setMat4("orthographic", orthographic);
    m_shader->setVec2("screenRatio", ratio);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_noiseTextureID);
    m_NoiseShader->setInt("noise", 0);

    m_NoiseShader->setFloat("noisePositions", noiseOffset1);
    m_NoiseShader->setFloat("scale", 0.4f);
    m_NoiseShader->setVec3("color", 0.9f, 0.9f, 0.2f);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4); // Render the quad


    m_NoiseShader->setFloat("noisePositions", noiseOffset2);
    m_NoiseShader->setFloat("scale", 0.3f);
    m_NoiseShader->setVec3("color", 0.0f, 0.5f, 0.2f);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4); // Render the quad


    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

}

void BackGround::Update(float p_deltaTime)
{
    noiseOffset1 += p_deltaTime / 120.0f;
    noiseOffset2 += p_deltaTime / 40.0f;

    if (noiseOffset1 > 3)
    {
        noiseOffset1 = 0;
    }
    if (noiseOffset2 > 2)
    {
        noiseOffset2 = 0;
    }
}
