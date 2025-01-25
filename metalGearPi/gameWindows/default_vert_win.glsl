#version 330 core

layout(location = 0) in vec3 aPos;       // Vertex position
layout(location = 1) in vec3 aNormal;    // Vertex normal (not used but kept for compatibility)
layout(location = 2) in vec2 aTexCoords; // Texture coordinates

out vec2 TexCoords;                     // Pass texture coordinates to fragment shader
out vec3 FragPos;                       // Pass world position to fragment shader
out vec3 Normal;

uniform mat4 MVP;                       // Model-View-Projection matrix
uniform mat4 model;                     // Model matrix

void main()
{
    // Pass the texture coordinates to the fragment shader
    TexCoords = aTexCoords;

    Normal = mat3(transpose(inverse(model))) * aNormal;
    // Calculate and pass the world position of the vertex
    FragPos = vec3(model * vec4(aPos, 1.0));

    // Transform vertex position using the MVP matrix
    gl_Position = MVP * vec4(aPos, 1.0);
}
