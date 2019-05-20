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
	vec4 diffuse_color = texture(samplerColor, inUV, 0.0);
	vec3 light_color = vec3(1, 1 ,1);
	vec3 ambient_color = vec3(1, 1, 1);

	vec3 light_dir = pos.lightPos.xyz - inFragWorldPos;
	float distance = length(light_dir);

	vec3 L = light_dir / distance;
	vec3 N = normalize(inNormal);
	vec3 V = normalize(pos.cameraPos.xyz - inFragWorldPos);
	vec3 R = normalize(reflect(-L, N));
	vec3 H = normalize(L + V);

	distance *= distance;

	float inv_pi = 0.318309886;

	float Kd = 100.0;
	float Ks = 20.0;

	float n_dot_l = max(dot(N, L), 0.0);
	float r_dot_v = max(dot(R, V), 0.0);
	float n_dot_h = max(dot(N, H), 0.0);

	float ambient  = 0.2;
	float diffuse  = Kd * n_dot_l / distance;
	float specular = Ks * pow(n_dot_h, 15.0) / distance;

	vec3 radiance  = diffuse_color.rgb * ambient +
					 diffuse_color.rgb * diffuse +
					 light_color * specular;

	outFragColor = vec4(radiance, diffuse_color.a);
}