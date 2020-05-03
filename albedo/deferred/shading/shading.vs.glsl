#version 450 core

layout (location = 0) in vec3 v_pos;

out struct VS_OUT
{
	vec2 v_uv;
} vs_out;


void main()
{
	vs_out.v_uv = vec2(0.5) + 0.5 * v_pos.xy;
	gl_Position = vec4(v_pos, 1);
}