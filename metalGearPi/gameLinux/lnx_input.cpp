#include "lnx_input.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include "../shared/baseInput.h"
#include <dirent.h> // For opendir(), readdir(), closedir()
#include <poll.h>

#include <X11/keysymdef.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>

// Constructor
LnxInput::LnxInput()
{
    m_running = true;
    // Detect keyboard and mouse devices
    //m_keyboardDevice = "/dev/input/" + FindInputDevice("keyboard");
    m_keyboardDevice = "/dev/input/event1"; // for some reason linux defaults to event2 instead of event1 which isnt the one needed

    m_mouseDevice = "/dev/input/" + FindInputDevice("mouse");

    // Log detected devices for debugging
    std::cout << "Keyboard Device: " << m_keyboardDevice << std::endl;
    std::cout << "Mouse Device: " << m_mouseDevice << std::endl;

    // Validate detected devices
    if (m_keyboardDevice.empty() || m_mouseDevice.empty())
    {
        std::cerr << "Error: Failed to find valid input devices!" << std::endl;
        m_running = false;
        return;
    }

    // Start the input thread
    m_inputThread = std::thread(&LnxInput::InputThread, this);
}

// Destructor
LnxInput::~LnxInput()
{
    // Stop the input thread safely
    m_running = false;
    if (m_inputThread.joinable())
    {
        m_inputThread.join();
    }
}

// Find Input Device
std::string LnxInput::FindInputDevice(const std::string& deviceType)
{
    DIR* dir = opendir("/dev/input/");
    if (!dir)
    {
        std::cerr << "Error: Unable to open /dev/input directory" << std::endl;
        return "";
    }

    struct dirent* entry;
    std::string eventFile;

    while ((entry = readdir(dir)) != nullptr)
    {
        // Only consider files named "event*"
        if (std::string(entry->d_name).find("event") != std::string::npos)
        {
            std::string eventPath = "/dev/input/" + std::string(entry->d_name);

            // Open the event file
            int fd = open(eventPath.c_str(), O_RDONLY);
            if (fd < 0)
            {
                continue; // Skip if the file cannot be opened
            }

            // Get the device name
            char name[256] = "Unknown";
            ioctl(fd, EVIOCGNAME(sizeof(name)), name);
            close(fd);

            // Match the device type (keyboard or mouse)
            if (deviceType == "keyboard")
            {
                if (std::string(name).find("Keyboard") != std::string::npos ||
                    std::string(name).find("keyboard") != std::string::npos ||
                    std::string(name).find("kbd") != std::string::npos ||
                    std::string(name).find("Input") != std::string::npos) // Broader match
                {
                    eventFile = std::string(entry->d_name);
                    std::cout << "Detected Keyboard: " << name << " -> " << eventFile << std::endl;
                    break;
                }
            }

            else if (deviceType == "mouse" &&
                (std::string(name).find("Mouse") != std::string::npos ||
                    std::string(name).find("mouse") != std::string::npos ||
                    std::string(name).find("pointer") != std::string::npos))
            {
                eventFile = std::string(entry->d_name);
                std::cout << "Detected Mouse: " << name << " -> " << eventFile << std::endl;
                break;
            }
        }
    }

    closedir(dir);
    return eventFile;
}

// Input Thread
void LnxInput::InputThread()
{
    int keyboardFd = open(m_keyboardDevice.c_str(), O_NONBLOCK);
    int mouseFd = open(m_mouseDevice.c_str(), O_NONBLOCK);

    if (keyboardFd < 0 || mouseFd < 0)
    {
        std::cerr << "Error: Unable to open input devices" << std::endl;
        m_running = false;
        return;
    }

    struct pollfd fds[2];
    fds[0].fd = keyboardFd;
    fds[0].events = POLLIN;
    fds[1].fd = mouseFd;
    fds[1].events = POLLIN;

    struct input_event event;

    while (m_running)
    {
        int pollResult = poll(fds, 2, -1); // Wait indefinitely for an event
        if (pollResult > 0)
        {

            // Check keyboard events
            if (fds[0].revents & POLLIN)
            {
                while (read(keyboardFd, &event, sizeof(event)) > 0)
                {
                    if (event.type == EV_KEY) // Handle key press/release
                    {
                        m_keyState[event.code] = event.value > 0; // Pressed or released
                        std::cout << "Keyboard Event - Key: " << event.code
                        << " State: " << event.value << std::endl;
                    }
                }
            }

            // Check mouse events
            if (fds[1].revents & POLLIN)
            {
                while (read(mouseFd, &event, sizeof(event)) > 0)
                {
                    if (event.type == EV_REL)
                    {
                        if (event.code == REL_X)
                            m_mouseState.x += event.value;
                        if (event.code == REL_Y)
                            m_mouseState.y += event.value;
                    }
                    else if (event.type == EV_KEY)
                    {
                        if (event.value == 1) // Button pressed
                        {
                            if (event.code == BTN_LEFT)
                                m_mouseState.buttonState |= BaseInput::MouseFlag::MOUSE_BTN_L;
                            if (event.code == BTN_RIGHT)
                                m_mouseState.buttonState |= BaseInput::MouseFlag::MOUSE_BTN_R;
                            if (event.code == BTN_MIDDLE)
                                m_mouseState.buttonState |= BaseInput::MouseFlag::MOUSE_BTN_M;
                        }
                        else if (event.value == 0) // Button released
                        {
                            if (event.code == BTN_LEFT)
                                m_mouseState.buttonState &= ~BaseInput::MouseFlag::MOUSE_BTN_L;
                            if (event.code == BTN_RIGHT)
                                m_mouseState.buttonState &= ~BaseInput::MouseFlag::MOUSE_BTN_R;
                            if (event.code == BTN_MIDDLE)
                                m_mouseState.buttonState &= ~BaseInput::MouseFlag::MOUSE_BTN_M;
                        }

                        std::cout << "Mouse Event - Button: " << event.code
                            << " State: " << event.value << std::endl;
                    }
                }
            }

        }
    }

    close(keyboardFd);
    close(mouseFd);
}

// Check if a key is pressed
bool LnxInput::IsKeyPressed(KeyBoardKey key) const
{
    // Translate the KeyBoardKey enum to its corresponding Linux key code
    int linuxKeyCode = TranslateKeyBoardKeyToLinuxCode(key);

    // Check if the key exists in the map and is currently pressed
    auto it = m_keyState.find(linuxKeyCode);
    if (it != m_keyState.end())
    {
        return it->second > 0; // Check if the key state is "pressed" (value > 0)
    }

    return false; // Key is not pressed or not found
}


// Poll a single key press
BaseInput::KeyBoardKey LnxInput::PollKeyBoardInput()
{
    for (auto it = m_keyState.begin(); it != m_keyState.end(); ++it)
    {
        int key = it->first;
        bool state = it->second;

        if (state) // If key is pressed
        {
            switch (key)
            {
            case 30: return BaseInput::KeyBoardKey::_KEY_A; // 'A' key code in /dev/input
            case 48: return BaseInput::KeyBoardKey::_KEY_B; // 'B' key
            case 46: return BaseInput::KeyBoardKey::_KEY_C; // 'C' key
            case 32: return BaseInput::KeyBoardKey::_KEY_D; // 'D' key
            case 18: return BaseInput::KeyBoardKey::_KEY_E; // 'E' key
            case 33: return BaseInput::KeyBoardKey::_KEY_F; // 'F' key
            case 34: return BaseInput::KeyBoardKey::_KEY_G; // 'G' key
            case 35: return BaseInput::KeyBoardKey::_KEY_H; // 'H' key
            case 23: return BaseInput::KeyBoardKey::_KEY_I; // 'I' key
            case 36: return BaseInput::KeyBoardKey::_KEY_J; // 'J' key
            case 37: return BaseInput::KeyBoardKey::_KEY_K; // 'K' key
            case 38: return BaseInput::KeyBoardKey::_KEY_L; // 'L' key
            case 50: return BaseInput::KeyBoardKey::_KEY_M; // 'M' key
            case 49: return BaseInput::KeyBoardKey::_KEY_N; // 'N' key
            case 24: return BaseInput::KeyBoardKey::_KEY_O; // 'O' key
            case 25: return BaseInput::KeyBoardKey::_KEY_P; // 'P' key
            case 16: return BaseInput::KeyBoardKey::_KEY_Q; // 'Q' key
            case 19: return BaseInput::KeyBoardKey::_KEY_R; // 'R' key
            case 31: return BaseInput::KeyBoardKey::_KEY_S; // 'S' key
            case 20: return BaseInput::KeyBoardKey::_KEY_T; // 'T' key
            case 22: return BaseInput::KeyBoardKey::_KEY_U; // 'U' key
            case 47: return BaseInput::KeyBoardKey::_KEY_V; // 'V' key
            case 17: return BaseInput::KeyBoardKey::_KEY_W; // 'W' key
            case 45: return BaseInput::KeyBoardKey::_KEY_X; // 'X' key
            case 21: return BaseInput::KeyBoardKey::_KEY_Y; // 'Y' key
            case 44: return BaseInput::KeyBoardKey::_KEY_Z; // 'Z' key

                // Numbers
            case 11: return BaseInput::KeyBoardKey::_KEY_0; // '0' key
            case 2:  return BaseInput::KeyBoardKey::_KEY_1; // '1' key
            case 3:  return BaseInput::KeyBoardKey::_KEY_2; // '2' key
            case 4:  return BaseInput::KeyBoardKey::_KEY_3; // '3' key
            case 5:  return BaseInput::KeyBoardKey::_KEY_4; // '4' key
            case 6:  return BaseInput::KeyBoardKey::_KEY_5; // '5' key
            case 7:  return BaseInput::KeyBoardKey::_KEY_6; // '6' key
            case 8:  return BaseInput::KeyBoardKey::_KEY_7; // '7' key
            case 9:  return BaseInput::KeyBoardKey::_KEY_8; // '8' key
            case 10: return BaseInput::KeyBoardKey::_KEY_9; // '9' key

                // Arrow keys
            case 105: return BaseInput::KeyBoardKey::_KEY_LEFT;  // Left Arrow
            case 106: return BaseInput::KeyBoardKey::_KEY_RIGHT; // Right Arrow
            case 103: return BaseInput::KeyBoardKey::_KEY_UP;    // Up Arrow
            case 108: return BaseInput::KeyBoardKey::_KEY_DOWN;  // Down Arrow

            default: return BaseInput::KeyBoardKey::CURRENT_KEY_UNKNOWN;
            }
        }
    }
    return BaseInput::KeyBoardKey::CURRENT_KEY_UNKNOWN; // No key pressed
}




// Check if a mouse button is pressed
bool LnxInput::IsMouseButtonPressed(int button) const
{
    return m_mouseState.buttonState & (1 << button);
}

// Poll mouse state
BaseInput::MouseInfo LnxInput::PollMouse()
{
    return m_mouseState;
}


int LnxInput::TranslateKeyBoardKeyToLinuxCode(KeyBoardKey key) const
{
    switch (key)
    {
        case KeyBoardKey::_KEY_A: return 30;
        case KeyBoardKey::_KEY_B: return 48;
        case KeyBoardKey::_KEY_C: return 46;
        case KeyBoardKey::_KEY_D: return 32;
        case KeyBoardKey::_KEY_E: return 18;
        case KeyBoardKey::_KEY_F: return 33;
        case KeyBoardKey::_KEY_G: return 34;
        case KeyBoardKey::_KEY_H: return 35;
        case KeyBoardKey::_KEY_I: return 23;
        case KeyBoardKey::_KEY_J: return 36;
        case KeyBoardKey::_KEY_K: return 37;
        case KeyBoardKey::_KEY_L: return 38;
        case KeyBoardKey::_KEY_M: return 50;
        case KeyBoardKey::_KEY_N: return 49;
        case KeyBoardKey::_KEY_O: return 24;
        case KeyBoardKey::_KEY_P: return 25;
        case KeyBoardKey::_KEY_Q: return 16;
        case KeyBoardKey::_KEY_R: return 19;
        case KeyBoardKey::_KEY_S: return 31;
        case KeyBoardKey::_KEY_T: return 20;
        case KeyBoardKey::_KEY_U: return 22;
        case KeyBoardKey::_KEY_V: return 47;
        case KeyBoardKey::_KEY_W: return 17;
        case KeyBoardKey::_KEY_X: return 45;
        case KeyBoardKey::_KEY_Y: return 21;
        case KeyBoardKey::_KEY_Z: return 44;

        case KeyBoardKey::_KEY_0: return 11;
        case KeyBoardKey::_KEY_1: return 2;
        case KeyBoardKey::_KEY_2: return 3;
        case KeyBoardKey::_KEY_3: return 4;
        case KeyBoardKey::_KEY_4: return 5;
        case KeyBoardKey::_KEY_5: return 6;
        case KeyBoardKey::_KEY_6: return 7;
        case KeyBoardKey::_KEY_7: return 8;
        case KeyBoardKey::_KEY_8: return 9;
        case KeyBoardKey::_KEY_9: return 10;

        case KeyBoardKey::_KEY_LEFT: return 105;
        case KeyBoardKey::_KEY_RIGHT: return 106;
        case KeyBoardKey::_KEY_UP: return 103;
        case KeyBoardKey::_KEY_DOWN: return 108;

        default: return -1; // Unknown key
    }
}
