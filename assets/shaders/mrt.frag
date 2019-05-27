#version 450

layout (binding = 2) uniform m_Pos{
    vec4 lightPos;
    vec4 cameraPos;
} pos;

layout (binding = 3) uniform sampler2D samplerAlbedo;

layout (location = 0) in vec2 inUV;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec3 inFragWorldPos;

layout (location = 0) out vec4 outAlbedo;

void main()
{
    outAlbedo = texture(samplerAlbedo, inUV);
}