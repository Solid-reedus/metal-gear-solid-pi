//#include "win_shader.h"
#include "win_window.h"
#include "win_input.h"

#include "../shared/game.h"
#include "../shared/common.h"
#include "../shared/baseImgui.h"
#include "../shared/ShaderManager.h"

int main()
{
    WinWindow* window = new WinWindow();
    WinInput* input = new WinInput(window->GetGLFWWindow());
    ShaderManager* shaderManager = new ShaderManager();

    const char* glsl_version = "#version 100";  // Ensure compatibility with OpenGL ES 2.0
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplOpenGL3_Init(glsl_version);
    
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(SCREEN_WIDTH, SCREEN_HEIGHT);

    Game* game = new Game(dynamic_cast<BaseWindow*>(window), dynamic_cast<BaseInput*>(input), shaderManager);
    game->Run();
    delete game;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();

    return 1;
}

