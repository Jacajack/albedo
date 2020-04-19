#include <iostream>
#include <albedo/albedo.hpp>
#include <albedo/gl/window.hpp>

void foo()
{
	throw abd::traced_exception<std::runtime_error>("haha");
}

int main(int argc, char *argv[])
{
	// GLFW init
	assert(glfwInit() == GLFW_TRUE);

	// GLFW window init
	abd::gl::window win(640, 480, "test", nullptr, nullptr, {
		{GLFW_SAMPLES, 0},
		{GLFW_CONTEXT_VERSION_MAJOR, 4},
		{GLFW_CONTEXT_VERSION_MINOR, 5},
		{GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE},
		{GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE},
	});

	win.run_main_loop([]()
	{

	});


	// GLEW destruction
	glfwTerminate();

	return 0;
}