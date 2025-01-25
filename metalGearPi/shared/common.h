#pragma once

#ifdef LINUX_BUILD
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#endif // LINUX_BUILD

#ifdef WINDOWS_BUILD
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#endif // WINDOWS_BUILD


#ifdef LINUX_BUILD
#define WINDOW_NAME "crossPlatformGame - linux"
#define GAME_VERSION "PI"
#define START_DIR "../shared/"
#define ASSETS_DIR "../shared/assets/"
#define SCREEN_WIDTH 540
#define SCREEN_HEIGHT 360
#define FONT_SIZE 14
#define NOISE_TEXTURE_SIZE 512
#endif // LINUX_BUILD

#ifdef WINDOWS_BUILD
#define WINDOW_NAME "crossPlatformGame - windows"
#define GAME_VERSION "windows"
#define START_DIR ""
#define ASSETS_DIR "../../shared/assets/"
#define SCREEN_HEIGHT 720
#define SCREEN_WIDTH 1080
#define FONT_SIZE 28
#define NOISE_TEXTURE_SIZE 1024
#endif // WINDOWS_BUILD

#define ENABLE_VLD

#if defined(_DEBUG) && defined(ENABLE_VLD) && defined(WINDOWS_BUILD)
#include <vld.h>
#endif // DEBUG && ENABLE_VLD && WINDOWS_BUILD

#define PI 3.14159265358979323846

typedef unsigned int uint;
typedef unsigned char uchar;

