#version 330 core

in vec2 TexCoord;            // Interpolated texture coordinates from vertex shader
out vec4 FragColor;          // Output color

uniform sampler2D aTexture;   // Texture sampler

void main() 
{
    FragColor = texture(aTexture, TexCoord);
}
