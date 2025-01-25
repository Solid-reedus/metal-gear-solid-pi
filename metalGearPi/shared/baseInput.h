#pragma once



class BaseInput 
{
    public:
    enum class KeyBoardKey 
    {
        // Letters
        _KEY_A, _KEY_B, _KEY_C, _KEY_D, _KEY_E, _KEY_F, _KEY_G,
        _KEY_H, _KEY_I, _KEY_J, _KEY_K, _KEY_L, _KEY_M, _KEY_N,
        _KEY_O, _KEY_P, _KEY_Q, _KEY_R, _KEY_S, _KEY_T, _KEY_U,
        _KEY_V, _KEY_W, _KEY_X, _KEY_Y, _KEY_Z,

        // Numbers
        _KEY_0, _KEY_1, _KEY_2, _KEY_3, _KEY_4,
        _KEY_5, _KEY_6, _KEY_7, _KEY_8, _KEY_9,

        // Arrow keys
        _KEY_LEFT, _KEY_RIGHT, _KEY_UP, _KEY_DOWN,

        // Unknown key
        CURRENT_KEY_UNKNOWN,
    };

    enum MouseFlag
    {
        MOUSE_BTN_L = 1 << 0, 
        MOUSE_BTN_R = 1 << 1, 
        MOUSE_BTN_M = 1 << 2, 
    };



    struct MouseInfo 
    {
        int x, y;          
        int scroll;        
        int buttonState;   
    };

    virtual ~BaseInput() = default;

    // Keyboard input
    virtual KeyBoardKey PollKeyBoardInput() = 0;
    virtual bool IsKeyPressed(KeyBoardKey key) const = 0;

    // Mouse input
    virtual MouseInfo PollMouse() = 0;
    virtual bool IsMouseButtonPressed(int button) const = 0;

    protected:
    BaseInput() = default;
};
