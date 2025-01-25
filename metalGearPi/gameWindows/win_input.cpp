
#include "win_input.h"
#include <GLFW/glfw3.h>
#include "../shared/baseInput.h"

// Translate BaseInput::KeyBoardKey to GLFW key codes
int WinInput::TranslateKey(BaseInput::KeyBoardKey key) const
{
    switch (key) 
    {
        case BaseInput::KeyBoardKey::_KEY_A: return GLFW_KEY_A;
        case BaseInput::KeyBoardKey::_KEY_B: return GLFW_KEY_B;
        case BaseInput::KeyBoardKey::_KEY_C: return GLFW_KEY_C;
        case BaseInput::KeyBoardKey::_KEY_D: return GLFW_KEY_D;
        case BaseInput::KeyBoardKey::_KEY_E: return GLFW_KEY_E;
        case BaseInput::KeyBoardKey::_KEY_F: return GLFW_KEY_F;
        case BaseInput::KeyBoardKey::_KEY_G: return GLFW_KEY_G;
        case BaseInput::KeyBoardKey::_KEY_H: return GLFW_KEY_H;
        case BaseInput::KeyBoardKey::_KEY_I: return GLFW_KEY_I;
        case BaseInput::KeyBoardKey::_KEY_J: return GLFW_KEY_J;
        case BaseInput::KeyBoardKey::_KEY_K: return GLFW_KEY_K;
        case BaseInput::KeyBoardKey::_KEY_L: return GLFW_KEY_L;
        case BaseInput::KeyBoardKey::_KEY_M: return GLFW_KEY_M;
        case BaseInput::KeyBoardKey::_KEY_N: return GLFW_KEY_N;
        case BaseInput::KeyBoardKey::_KEY_O: return GLFW_KEY_O;
        case BaseInput::KeyBoardKey::_KEY_P: return GLFW_KEY_P;
        case BaseInput::KeyBoardKey::_KEY_Q: return GLFW_KEY_Q;
        case BaseInput::KeyBoardKey::_KEY_R: return GLFW_KEY_R;
        case BaseInput::KeyBoardKey::_KEY_S: return GLFW_KEY_S;
        case BaseInput::KeyBoardKey::_KEY_T: return GLFW_KEY_T;
        case BaseInput::KeyBoardKey::_KEY_U: return GLFW_KEY_U;
        case BaseInput::KeyBoardKey::_KEY_V: return GLFW_KEY_V;
        case BaseInput::KeyBoardKey::_KEY_W: return GLFW_KEY_W;
        case BaseInput::KeyBoardKey::_KEY_X: return GLFW_KEY_X;
        case BaseInput::KeyBoardKey::_KEY_Y: return GLFW_KEY_Y;
        case BaseInput::KeyBoardKey::_KEY_Z: return GLFW_KEY_Z;
        case BaseInput::KeyBoardKey::_KEY_LEFT: return GLFW_KEY_LEFT;
        case BaseInput::KeyBoardKey::_KEY_RIGHT: return GLFW_KEY_RIGHT;
        case BaseInput::KeyBoardKey::_KEY_UP: return GLFW_KEY_UP;
        case BaseInput::KeyBoardKey::_KEY_DOWN: return GLFW_KEY_DOWN;
        default: return -1; // Unsupported key
    }
}

// Check if a specific key is currently pressed
bool WinInput::IsKeyPressed(BaseInput::KeyBoardKey key) const 
{
    int glfwKey = TranslateKey(key);
    if (glfwKey == -1) 
    {
        return false; // Unsupported key
    }
    return glfwGetKey(m_window, glfwKey) == GLFW_PRESS;
}

// Poll for a single key input (simplified example)
BaseInput::KeyBoardKey WinInput::PollKeyBoardInput() 
{
    // This function can be customized to poll for specific key events
    for (int key = GLFW_KEY_A; key <= GLFW_KEY_Z; ++key) 
    {
        if (glfwGetKey(m_window, key) == GLFW_PRESS) 
        {
            return static_cast<BaseInput::KeyBoardKey>(key - GLFW_KEY_A + static_cast<int>(BaseInput::KeyBoardKey::_KEY_A));
        }
    }
    return BaseInput::KeyBoardKey::CURRENT_KEY_UNKNOWN;
}

// Check if a mouse button is currently pressed
bool WinInput::IsMouseButtonPressed(int button) const 
{
    return glfwGetMouseButton(m_window, button) == GLFW_PRESS;
}

// Poll for mouse input information
BaseInput::MouseInfo WinInput::PollMouse()
{
    BaseInput::MouseInfo mouseInfo = {};
    double xpos, ypos;

    // Get current mouse position
    glfwGetCursorPos(m_window, &xpos, &ypos);
    mouseInfo.x = static_cast<int>(xpos);
    mouseInfo.y = static_cast<int>(ypos);

    // Update button state using the MouseFlag enum
    mouseInfo.buttonState = 0;
    if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        mouseInfo.buttonState |= BaseInput::MouseFlag::MOUSE_BTN_L;
    if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
        mouseInfo.buttonState |= BaseInput::MouseFlag::MOUSE_BTN_R;
    if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
        mouseInfo.buttonState |= BaseInput::MouseFlag::MOUSE_BTN_M;

    return mouseInfo;
}
