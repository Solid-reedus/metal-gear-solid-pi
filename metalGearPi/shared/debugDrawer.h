#pragma once

#ifdef LINUX_BUILD
#include <bullet/btBulletDynamicsCommon.h>
#include <GLES3/gl3.h>
#endif // LINUX_BUILD

#ifdef WINDOWS_BUILD
#include <bullet/btBulletDynamicsCommon.h>
#include "glad/glad.h"
#endif // WINDOWS_BUILD

#include <vector>
#include "baseShader.h"
#include <iostream>



class DebugDrawer : public btIDebugDraw
{
    private:
    GLuint m_VAO, m_VBO;
    int m_debugMode;

    std::vector<GLfloat> m_lineVertices;

    BaseShader* m_lineShader; // Shader for drawing lines

    public:
    DebugDrawer(BaseShader* lineShader) : m_debugMode(DBG_DrawWireframe), m_lineShader(lineShader)
    {

        // Generate VAO and VBO for lines
        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);

        glBindVertexArray(m_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 0, nullptr, GL_DYNAMIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    ~DebugDrawer()
    {
        glDeleteBuffers(1, &m_VBO);
        glDeleteVertexArrays(1, &m_VAO);
    }

    // Implementation of drawLine
    virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3&) override
    {
        // Add line vertices to the vertex list
        m_lineVertices.push_back(from.getX());
        m_lineVertices.push_back(from.getY());
        m_lineVertices.push_back(from.getZ());

        m_lineVertices.push_back(to.getX());
        m_lineVertices.push_back(to.getY());
        m_lineVertices.push_back(to.getZ());
    }

    // Implementation of drawContactPoint
    virtual void drawContactPoint(const btVector3&, const btVector3&, btScalar, int, const btVector3&) override
    {
        // Optional: Add contact points visualization if needed
    }

    // Flush the lines to GPU for rendering
    void flushLines()
    {
        if (m_lineVertices.empty())
            return;

        glBindVertexArray(m_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

        // Upload the line data to the GPU
        glBufferData(GL_ARRAY_BUFFER, m_lineVertices.size() * sizeof(GLfloat), m_lineVertices.data(), GL_DYNAMIC_DRAW);

        // Use the line shader
        m_lineShader->use();

        // Draw the lines
        glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(m_lineVertices.size() / 3));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        // Clear the line vertices for the next frame
        m_lineVertices.clear();
    }

    // Implementation of reportErrorWarning
    virtual void reportErrorWarning(const char* warningString) override
    {
        std::cerr << "Bullet Warning: " << warningString << std::endl;
    }

    // Implementation of draw3dText
    virtual void draw3dText(const btVector3&, const char*) override
    {
        // Not implemented
    }

    // Implementation of setDebugMode
    virtual void setDebugMode(int debugMode) override
    {
        m_debugMode = debugMode;
    }

    // Implementation of getDebugMode
    virtual int getDebugMode() const override
    {
        return m_debugMode;
    }
};
