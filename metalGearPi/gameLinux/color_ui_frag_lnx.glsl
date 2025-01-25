#version 310 es
precision mediump float; // Specify default precision for float types

out vec4 FragColor;          // Output color

uniform vec3 color;

void main() 
{
    FragColor = vec4(color, 1.0f);
}
