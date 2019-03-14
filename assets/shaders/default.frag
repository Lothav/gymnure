#version 420

layout(location = 0) in vec2 inUV;

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = vec4(1 ,1, 1, 1);//texture(samplerColor, inUV, 0.0);
}