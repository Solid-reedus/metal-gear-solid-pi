#include "debugCameraController.h"

#ifdef LINUX_BUILD
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#endif // LINUX_BUILD

#ifdef WINDOWS_BUILD
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#endif // WINDOWS_BUILD

#include "camera.h"


DebugCameraController::DebugCameraController(Camera& p_camera) : m_camera(p_camera) {}

void DebugCameraController::ProcessKeyboard(Camera_Movement p_direction, float p_deltaTime)
{
    float velocity = m_movementSpeed * p_deltaTime;

    switch (p_direction)
    {
        case FORWARD:  m_camera.position += m_camera.front * velocity; break;
        case BACKWARD: m_camera.position -= m_camera.front * velocity; break;
        case LEFT:     m_camera.position -= m_camera.right * velocity; break;
        case RIGHT:    m_camera.position += m_camera.right * velocity; break;
        case UP:       m_camera.position += m_camera.WORLD_UP * velocity; break;
        case DOWN:     m_camera.position -= m_camera.WORLD_UP * velocity; break;
        default:
        break;
    }
}

void DebugCameraController::ProcessMouseMovement(glm::vec2 newMousePos, GLboolean constrainPitch)
{
    if (m_firstMouseInput)
    {
        m_lastMousePos = newMousePos;
        m_firstMouseInput = false;
        return;
    }

    glm::vec2 offset = newMousePos - m_lastMousePos;
    offset *= m_mouseSensitivity;

    m_camera.yaw += offset.x;
    m_camera.pitch -= offset.y; // Inverted Y-axis for natural behavior

    if (constrainPitch)
    {
        if (m_camera.pitch > 89.0f) m_camera.pitch = 89.0f;
        if (m_camera.pitch < -89.0f) m_camera.pitch = -89.0f;
    }

    m_lastMousePos = newMousePos;

    m_camera.UpdateCamVecs();
}

void DebugCameraController::ResetMousePosition(glm::vec2 initialMousePos)
{
    m_lastMousePos = initialMousePos;
    m_firstMouseInput = true;
}