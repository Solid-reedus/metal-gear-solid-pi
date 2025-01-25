#version 330 core

in vec2 TexCoord;            // Interpolated texture coordinates from vertex shader
out vec4 FragColor;          // Output color

uniform sampler2D noise; 
uniform vec3 color;
uniform float noisePositions;
uniform float scale;

void main() 
{
	vec2 noisecoord = TexCoord;
	noisecoord.x += noisePositions;
	FragColor = ((texture(noise, noisecoord * scale).xxxx * 1.8f) * vec4(color.x, color.y, color.z, 1.0f));
}
