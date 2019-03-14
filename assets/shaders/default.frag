#version 450

layout(binding = 1) uniform sampler2D samplerColor;

layout(location = 0) in vec2 inUV;

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = texture(samplerColor, inUV, 0.0);
}