#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#ifdef LINUX_BUILD
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // For matrix operations
#include <glm/gtx/transform2.hpp>       // For 2D matrix operations
#endif // LINUX_BUILD

#ifdef WINDOWS_BUILD
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"     // For matrix operations
#include "gtx/transform2.hpp"           // For 2D matrix operations
#endif // WINDOWS_BUILD

namespace Mat3Utils 
{

    inline glm::mat3 translate(const glm::vec2& translation) 
    {
        glm::mat3 result(1.0f); // Identity matrix
        result[2][0] = translation.x;
        result[2][1] = translation.y;
        return result;
    }

    inline glm::mat3 scale(const glm::vec2& scale) 
    {
        glm::mat3 result(1.0f); // Identity matrix
        result[0][0] = scale.x; // Scale x-axis
        result[1][1] = scale.y; // Scale y-axis
        return result;
    }

    inline glm::mat3 rotate(float angle) 
    {
        glm::mat3 result(1.0f); // Identity matrix
        float cosTheta = cos(angle);
        float sinTheta = sin(angle);

        result[0][0] = cosTheta;  result[0][1] = sinTheta;
        result[1][0] = -sinTheta; result[1][1] = cosTheta;

        return result;
    }

    inline glm::mat3 translate(glm::mat3 mat, const glm::vec2& translation)
    {
        glm::mat3 result = mat; // Start with the current matrix
        result[2][0] += translation.x; // Apply translation on the x-axis
        result[2][1] += translation.y; // Apply translation on the y-axis
        return result;
    }

    inline glm::mat3 scale(glm::mat3 mat, const glm::vec2& scale)
    {
        glm::mat3 result = mat; // Start with the current matrix
        result[0][0] *= scale.x; // Scale x-axis
        result[1][1] *= scale.y; // Scale y-axis
        return result;
    }

    inline glm::mat3 rotate(glm::mat3 mat, float angle)
    {
        glm::mat3 result = mat; // Start with the current matrix
        float cosTheta = cos(angle);
        float sinTheta = sin(angle);

        // Apply rotation
        result[0][0] = cosTheta * mat[0][0] + sinTheta * mat[0][1];
        result[0][1] = -sinTheta * mat[0][0] + cosTheta * mat[0][1];
        result[1][0] = cosTheta * mat[1][0] + sinTheta * mat[1][1];
        result[1][1] = -sinTheta * mat[1][0] + cosTheta * mat[1][1];

        return result;
    }

    //inline glm::mat3 combine(const glm::vec2& translation, const glm::vec2& scale, float rotation) {
    //    glm::mat3 transMat = translate(translation);
    //    glm::mat3 scaleMat = scale(scale);
    //    glm::mat3 rotMat = rotate(rotation);
    //
    //    return transMat * rotMat * scaleMat; // Order: scale, rotate, then translate
    //}
}

