#include "lnx_window.h"
#include "lnx_input.h"
//#include "lnx_shader.h"

#include "game.h"
#include "common.h"

#include "../shared/baseImgui.h"

#include <unistd.h>
#include <iostream>
#include <cstdio>
#include "../shared/ShaderManager.h"

int main()
{
    LnxWindow* window = new LnxWindow();
    LnxInput* input = new LnxInput();
    ShaderManager* shaderManager = new ShaderManager();


    const char* glsl_version = "#version 100";  // Ensure compatibility with OpenGL ES 2.0
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplOpenGL3_Init(glsl_version);
    
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(SCREEN_WIDTH, SCREEN_HEIGHT);

    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != nullptr) 
    {
        std::cout << "Current working directory: " << cwd << std::endl;
    }


    std::string filePath = std::string(ASSETS_DIR) + "grassPlane/grassPlane.model";
    printf("Loading model: %s\n", filePath.c_str());


    Game* game = new Game(dynamic_cast<BaseWindow*>(window), dynamic_cast<BaseInput*>(input), shaderManager);
    game->Run();
    delete game;

    //ImGui_ImplOpenGL3_Shutdown();
    //ImGui::DestroyContext();

    return 1;
}
