#version 450 core

// Standard input attributes layout
layout (location = 0) in vec3 v_pos;
layout (location = 1) in vec3 v_normal;
layout (location = 2) in vec2 v_uv;

uniform mat4 mat_model;
uniform mat4 mat_view;
uniform mat4 mat_proj;
uniform mat4 mat_vp;

struct VS_OUT
{
	vec3 v_pos;
	vec3 v_normal;
} vs_out;


void main()
{
	vs_out.v_pos = (mat_vp * mat_model * vec4(v_pos, 1)).xyz;
	vs_out.v_normal = (mat_vp * mat_model * vec4(v_normal, 0)).xyz;

	gl_Position = mat_vp * mat_model * vec4(v_pos, 1);
}