#version 450 core

// Standard layout for deferred rendering
layout (location = 0) out vec3 f_color;
layout (location = 1) out vec3 f_pos;
layout (location = 2) out vec3 f_normal;
layout (location = 3) out vec3 f_diffuse;
layout (location = 4) out vec3 f_specular;

in struct VS_OUT
{
	vec3 v_pos;
	vec3 v_normal;
} vs_out;

void main()
{
	f_pos = vs_out.v_pos;
	f_normal = vs_out.v_normal;
	f_color = f_normal; // debug
}