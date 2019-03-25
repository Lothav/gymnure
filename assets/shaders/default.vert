#version 420

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (binding = 0) uniform UBO{
    mat4 m;
    mat4 v;
    mat4 p;
} ubu;

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inUV;

layout (location = 0) out vec2 outUV;

void main()
{
    outUV       = inUV;
    gl_Position = ubu.p * ubu.v * ubu.m * vec4(inPos.xyz, 1.0);
}
