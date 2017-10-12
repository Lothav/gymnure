#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (binding = 0) uniform UBO {
    mat4 view;
    mat4 model;
    mat4 projection;
} ubo;

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec3 inNormal;

layout (location = 0) out vec2 outUV;

void main() {
   outUV = inUV;
   gl_Position = ubo.view * ubo.model * vec4(inPos.xyz, 1.0) ;
}