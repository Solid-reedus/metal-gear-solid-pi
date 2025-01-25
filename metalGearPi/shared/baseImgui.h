#pragma once

#include "ImGui-master/imgui.h"
#include "ImGui-master/backends/imgui_impl_opengl3.h"

//#include "../../../shared/ImGui-master/imgui.h"
//#include "../../../shared/ImGui-master/backends/imgui_impl_opengl3.h"

#include "common.h"
#include <vector>
#include <string>

//void ImGuiInit()
//{
//    const char* glsl_version = "#version 330";  // Ensure compatibility with OpenGL ES 2.0
//    ImGui::CreateContext();
//    ImGui::StyleColorsDark();
//    ImGui_ImplOpenGL3_Init(glsl_version);
//
//    ImGuiIO& io = ImGui::GetIO();
//    io.DisplaySize = ImVec2(SCREEN_WIDTH, SCREEN_HEIGHT);
//}

class BaseImgui
{
    public:

    virtual ~BaseImgui();

    void Draw();

    protected:
    // this is a virtual funtion that a class can override in order to do
    // its own costum imgui draw
    virtual void CostumDraw() = 0;

    private:
    void ImguiBegin();

    void ImguiEnd()
    {
        ImGui::EndFrame();
    }
};

class TextRendererImgui : public BaseImgui
{
    public:
    TextRendererImgui(const std::vector<std::string>& p_text, float p_fontScale);

    std::vector<std::string> m_text;

    protected:
    void CostumDraw() override;

    private:
    float m_fontScale;
};

