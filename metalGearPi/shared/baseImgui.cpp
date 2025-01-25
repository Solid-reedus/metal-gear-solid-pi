#include "baseImgui.h"

#include "ImGui-master/imgui.h"
#include "ImGui-master/backends/imgui_impl_opengl3.h"

BaseImgui::~BaseImgui()
{
    //ImGui_ImplOpenGL3_Shutdown();
    //ImGui::DestroyContext();
}

void BaseImgui::Draw()
{
    ImguiBegin();
    CostumDraw();
    ImguiEnd();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void BaseImgui::ImguiBegin()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
    
}

TextRendererImgui::TextRendererImgui(const std::vector<std::string>& p_text, float p_fontScale)
    : m_text(p_text), m_fontScale(p_fontScale) {}

void TextRendererImgui::CostumDraw()
{
    // Backup current style
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec2 originalWindowPadding = style.WindowPadding;
    ImVec2 originalItemSpacing = style.ItemSpacing;
    ImVec2 originalFramePadding = style.FramePadding;

    // Set custom style for minimal padding and spacing
    style.WindowPadding = ImVec2(0.0f, 0.0f); // Remove window padding
    style.ItemSpacing = ImVec2(0.0f, 0.0f);   // Remove spacing between items
    style.FramePadding = ImVec2(0.0f, 0.0f);  // Remove internal padding in text frames

    // Begin a new window with no decoration or padding
    ImGui::Begin("debug info", nullptr,
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_AlwaysAutoResize);

    // Set the font scale
    ImGui::SetWindowFontScale(m_fontScale);

    // Draw text items
    for (std::string& text : m_text)
    {
        ImGui::TextUnformatted(text.c_str());
    }

    ImGui::End();

    // Restore the original style
    style.WindowPadding = originalWindowPadding;
    style.ItemSpacing = originalItemSpacing;
    style.FramePadding = originalFramePadding;
}
