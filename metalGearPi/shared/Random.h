#pragma once

#include "common.h"

#ifdef LINUX_BUILD
#include "stb/stb_image.h"
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#endif // LINUX_BUILD

#ifdef WINDOWS_BUILD
#include "../libraries/windows/stb/stb_image.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#endif // WINDOWS_BUILD

inline uint RandomUint(uint& seed)
{
    seed ^= seed << 13;
    seed ^= seed >> 17;
    seed ^= seed << 5;
    return seed;
}

inline float RandomFloat(uint& seed)
{
    return static_cast<float>(static_cast<float>(RandomUint(seed)) * 2.3283064365387e-10f);
}

inline void diamondSquare(uchar* data, int width, int height, uint& seed, float scale)
{
    int tempWidth = width + 1;
    int tempHeight = height + 1;
    uchar* tempData = new uchar[static_cast<size_t>(tempWidth) * static_cast<size_t>(tempHeight)];

    // Initialize tempData to zero
    for (int i = 0; i < static_cast<int>(tempWidth * tempHeight); i++)
    {
        tempData[i] = 0;
    }

    // Initialize corners
    uchar randNum = static_cast<uchar>(RandomFloat(seed) * 255);
    tempData[0] = randNum;
    tempData[tempWidth - 1] = randNum;
    tempData[(tempHeight - 1) * tempWidth] = randNum;
    tempData[(tempHeight - 1) * tempWidth + (tempWidth - 1)] = randNum;

    int currentSize = tempWidth - 1;

    while (currentSize > 1)
    {
        int halfStep = currentSize / 2;

        // Diamond Step
        for (int y = 0; y < tempHeight - 1; y += currentSize)
        {
            for (int x = 0; x < tempWidth - 1; x += currentSize)
            {
                int mid = (y + halfStep) * tempWidth + (x + halfStep);
                int tl = y * tempWidth + x;
                int tr = y * tempWidth + (x + currentSize);
                int bl = (y + currentSize) * tempWidth + x;
                int br = (y + currentSize) * tempWidth + (x + currentSize);

                float avg = static_cast<float>(tempData[tl] + tempData[tr] + tempData[bl] + tempData[br]) / 4.0f;
                tempData[mid] = static_cast<uchar>(glm::clamp(avg + (RandomFloat(seed) - 0.5f) * scale, 0.0f, 255.0f));
            }
        }

        // Square Step
        for (int y = 0; y < tempHeight; y += halfStep)
        {
            for (int x = (y + halfStep) % currentSize; x < tempWidth; x += currentSize)
            {
                float avg = 0.0f;
                int count = 0;

                if (x - halfStep >= 0)
                {
                    avg += tempData[y * tempWidth + (x - halfStep)];
                    count++;
                }
                if (x + halfStep < tempWidth)
                {
                    avg += tempData[y * tempWidth + (x + halfStep)];
                    count++;
                }
                if (y - halfStep >= 0)
                {
                    avg += tempData[(y - halfStep) * tempWidth + x];
                    count++;
                }
                if (y + halfStep < tempHeight)
                {
                    avg += tempData[(y + halfStep) * tempWidth + x];
                    count++;
                }

                avg /= static_cast<float>(count);
                tempData[y * tempWidth + x] = static_cast<uchar>(glm::clamp(avg + (RandomFloat(seed) - 0.5f) * scale, 0.0f, 255.0f));
            }
        }

        currentSize /= 2;
    }

    // Copy clipped region to the final buffer
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            data[y * width + x] = tempData[y * tempWidth + x];
        }
    }

    delete[] tempData;
}

inline uint createNoiseTexture(int p_size, uint seed, float noiseScale)
{
    uchar* noisedata = new uchar[static_cast<size_t>(p_size) * static_cast<size_t>(p_size)];
    diamondSquare(noisedata, p_size, p_size, seed, noiseScale);

    uint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RED, // Single channel (grayscale)
        static_cast<GLsizei>(p_size),
        static_cast<GLsizei>(p_size),
        0,
        GL_RED,
        GL_UNSIGNED_BYTE,
        noisedata
    );

    glBindTexture(GL_TEXTURE_2D, 0);

    delete[] noisedata;

    return textureID;
}
