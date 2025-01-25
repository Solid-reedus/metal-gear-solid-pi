#version 310 es
precision mediump float;

layout(location = 0) in vec2 aPos;       // Position of the vertex (from the quad)
layout(location = 1) in vec2 aTexCoord;  // Texture coordinates

uniform mat3 model;                      // Model transformation matrix

out vec2 TexCoord;                       // Pass UV to fragment shader

void main() 
{
    vec3 screenPos = model * vec3(aPos, 1.0);    // Apply model transformation
    gl_Position = vec4(screenPos.xy, 0.0, 1.0);  // Directly output screen position in clip space
    TexCoord = aTexCoord;                        // Pass through texture coordinates
}
