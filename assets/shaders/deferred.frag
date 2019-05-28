#version 420

layout(binding = 0) uniform sampler2D samplerAlbedo;

layout(location = 0) in vec2 inUV;
layout(location = 0) out vec4 outColor;

void main()
{
    outColor = texture(samplerAlbedo, inUV, 0.0);
}