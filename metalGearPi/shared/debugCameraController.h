#pragma once

#ifdef LINUX_BUILD
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <X11/Xlib.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>

#endif // LINUX_BUILD

#ifdef WINDOWS_BUILD
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#endif // WINDOWS_BUILD

#include "camera.h"
#include "baseInput.h"


class DebugCameraController
{
    public:
    enum Camera_Movement
    {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
        UP,
        DOWN
    };

    DebugCameraController(Camera& p_camera);
    ~DebugCameraController() {};

    void ProcessKeyboard(Camera_Movement p_direction, float p_deltaTime);
    void ProcessMouseMovement(glm::vec2 newMousePos, GLboolean constrainPitch);
    void ResetMousePosition(glm::vec2 initialMousePos);

    private:
    Camera& m_camera;
    // default camera options
    float m_movementSpeed = 5.0f;
    float m_mouseSensitivity = 0.07f;

    glm::vec2 m_lastMousePos = glm::vec2(0, 0);
    bool m_firstMouseInput = false;
};

