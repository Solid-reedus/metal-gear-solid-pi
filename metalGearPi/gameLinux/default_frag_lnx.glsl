#version 310 es

precision mediump float;

struct Sun 
{
    vec3 dir;
    vec4 colorAndIntensity;
};

struct SpotLight 
{
    vec3 pos;
    vec3 dir;
    vec4 colorAndIntensity;
};

struct PointLight 
{
    vec3 pos;
    vec4 colorAndIntensity;
};

#define AMBIENT 0.2
#define MAX_SUNS 1
#define MAX_SPOTLIGHTS 5
#define MAX_AREALIGHTS 5

layout(std140) uniform Lighting 
{
    Sun suns[MAX_SUNS];
    SpotLight spotlights[MAX_SPOTLIGHTS];
    PointLight pointLights[MAX_AREALIGHTS];
    vec3 numOfLights; // x: numSuns, y: numPointLights, z: numSpotlights
};

in vec3 FragPos;     // World-space position
in vec2 TexCoords;   // Texture coordinates
in vec3 Normal;      // Surface normal

out vec4 FragColor;

uniform vec3 viewPos;
uniform sampler2D textureDiffuse;

vec3 CalcSunLight(Sun light, vec3 normal, vec3 diffuseColor);
vec3 CalcPointLight(PointLight light, vec3 fragPos, vec3 normal, vec3 diffuseColor);
vec3 CalcSpotLight(SpotLight light, vec3 fragPos, vec3 normal, vec3 diffuseColor);

void main() 
{

    vec3 diffuseColor = vec3(texture(textureDiffuse, TexCoords));
    vec3 result = AMBIENT * diffuseColor;

    // Normalize the interpolated normal
    vec3 norm = normalize(Normal);

    // Add Sun lights
    for (int i = 0; i < int(numOfLights.x); i++) 
    {
        result += CalcSunLight(suns[i], norm, diffuseColor);
    }

    // Add Point lights
    for (int i = 0; i < int(numOfLights.z); i++) 
    {
        result += CalcPointLight(pointLights[i], FragPos, norm, diffuseColor);
    }

    // Add Spotlights
    for (int i = 0; i < int(numOfLights.y); i++) 
    {
        result += CalcSpotLight(spotlights[i], FragPos, norm, diffuseColor);
    }

    FragColor = vec4(result, 1.0);
}

vec3 CalcSunLight(Sun light, vec3 normal, vec3 diffuseColor) 
{
    vec3 lightDir = normalize(-light.dir);
    float diff = max(dot(normal, lightDir), 0.0);
    float intensity = light.colorAndIntensity.w;
    vec3 lightColor = light.colorAndIntensity.rgb;

    return diff * intensity * lightColor * diffuseColor;
}

vec3 CalcPointLight(PointLight light, vec3 fragPos, vec3 normal, vec3 diffuseColor) 
{
    vec3 lightDir = normalize(light.pos - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    float distance = length(light.pos - fragPos);
    float attenuation = 1.0 / (distance * distance);
    float intensity = light.colorAndIntensity.w * attenuation;
    vec3 lightColor = light.colorAndIntensity.rgb;

    return diff * intensity * lightColor * diffuseColor;
}

vec3 CalcSpotLight(SpotLight light, vec3 fragPos, vec3 normal, vec3 diffuseColor) 
{
    vec3 lightDir = normalize(light.pos - fragPos);
    float theta = dot(lightDir, normalize(-light.dir));
    float diff = max(dot(normal, lightDir), 0.0);
    float intensity = light.colorAndIntensity.w;

    float innerCutoff = 0.9;
    float outerCutoff = 0.8;
    float epsilon = innerCutoff - outerCutoff;
    float spotIntensity = clamp((theta - outerCutoff) / epsilon, 0.0, 1.0);

    vec3 lightColor = light.colorAndIntensity.rgb;

    return diff * intensity * spotIntensity * lightColor * diffuseColor;
}
