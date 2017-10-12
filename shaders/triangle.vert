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
layout (location = 2) out vec3 outNormal;
layout (location = 3) out vec3 outViewVec;
layout (location = 4) out vec3 outLightVec;

void main() {

   	outUV = inUV;
    gl_Position = ubo.view * ubo.model * vec4(inPos.xyz, 1.0) ;

    vec4 pos = ubo.model * vec4(inPos, 1.0);
   	outNormal = mat3(inverse(transpose(ubo.model))) * inNormal;
   	vec3 lightPos = vec3(0.0, 0.0, -100.0);
   	vec3 lPos = mat3(ubo.model) * lightPos.xyz;
    outLightVec = lPos - pos.xyz;
    outViewVec = (ubo.view * vec4(1.0)).xyz - pos.xyz;
}