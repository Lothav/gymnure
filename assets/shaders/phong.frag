#version 420

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (binding = 2) uniform m_Pos{
	vec4 lightPos;
	vec4 cameraPos;
} pos;

layout (binding = 3) uniform sampler2D samplerColor;

layout (location = 0) in vec2 inUV;
layout (location = 1) in vec3 inFragWorldPos;
layout (location = 2) in vec3 inNormal;

layout (location = 0) out vec4 outFragColor;

void main()
{
	vec4 color = texture(samplerColor, inUV, 0.0);
	color = vec4(0.1, 0.1, 0.1, 1);

	vec3 N = normalize(inNormal);
	vec3 L = normalize(pos.lightPos.xyz  - inFragWorldPos);
	vec3 V = normalize(pos.cameraPos.xyz - inFragWorldPos);
	vec3 R = normalize(reflect(-L, N));

	float inv_pi = 0.318309886;

	float Kd = 5.0;
	float Ks = 100.0;

	float n_dot_l = max(dot(N, L), 0.0);
	float r_dot_v = max(dot(R, V), 0.0);

	float ambient  = 0.1;
	float diffuse  = Kd * inv_pi;
	float specular = Ks * inv_pi * pow(r_dot_v, 4.0);

	vec3 brdf     = color.rgb * diffuse + specular * vec3(0, 0 ,1); // Evaluate the complete Phong BRDF.
	vec3 radiance = brdf * n_dot_l; // Combine the BRDF and the irradiance.

	outFragColor = vec4(radiance, color.a);
}