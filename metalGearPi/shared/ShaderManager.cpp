#include "ShaderManager.h"

#ifdef LINUX_BUILD
#include "../gameLinux/lnx_shader.h"
#endif // LINUX_BUILD

#ifdef WINDOWS_BUILD
#include "../gameWindows/win_shader.h"
#endif // WINDOWS_BUILD

ShaderManager::ShaderManager()
{
	#ifdef LINUX_BUILD 
	shaders[0] = new LnxShader("../gameLinux/default_vert_lnx.glsl", "../gameLinux/default_frag_lnx.glsl");		  // defaultShader
	shaders[2] = new LnxShader("../gameLinux/default_ui_vert_lnx.glsl", "../gameLinux/color_ui_frag_lnx.glsl");	  // uiColorShader
	shaders[3] = new LnxShader("../gameLinux/default_ui_vert_lnx.glsl", "../gameLinux/background_frag_lnx.glsl"); // noiseUiShader
	shaders[4] = new LnxShader("../gameLinux/textShader_vert_lnx.glsl", "../gameLinux/textShader_frag_lnx.glsl"); // textShader   
	
	
	shaders[1] = new LnxShader("../gameLinux/default_ui_vert_lnx.glsl", "../gameLinux/default_ui_frag_lnx.glsl"); // uiShader
	#endif // LINUX_BUILD
	
	#ifdef WINDOWS_BUILD
	shaders[0] = new WinShader("../default_vert_win.glsl", "../default_frag_win.glsl");		  // defaultShader
	shaders[1] = new WinShader("../default_ui_vert_win.glsl", "../default_ui_frag_win.glsl"); // uiShader
	shaders[2] = new WinShader("../default_ui_vert_win.glsl", "../color_ui_frag_win.glsl");   // uiColorShader
	shaders[3] = new WinShader("../default_ui_vert_win.glsl", "../background_frag_win.glsl"); // noiseUiShader
	shaders[4] = new WinShader("../textShader_vert_win.glsl", "../textShader_frag_win.glsl"); // textShader
	#endif // WINDOWS_BUILD
}



void ShaderManager::Use(ShaderEnum p_shader)
{
	shaders[static_cast<int>(p_shader)]->use();
}

BaseShader* ShaderManager::GetShader(ShaderEnum p_shader)
{
	return shaders[static_cast<int>(p_shader)];
}
