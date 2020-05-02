#version 450 core

// Standard layout for deferred rendering
layout (location = 0) out vec3 f_color;
layout (location = 1) out vec3 f_pos;

void main()
{
	f_color = vec3(1.0);
	f_pos = vec3(0,1,0);

}