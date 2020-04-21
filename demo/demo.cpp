#include <iostream>
#include <albedo/albedo.hpp>
#include <albedo/gl/window.hpp>
#include <albedo/gl/gl_object.hpp>
#include <albedo/gl/program.hpp>
#include <albedo/gl/debug.hpp>
#include <albedo/gl/vertex_array.hpp>
#include <memory>

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
		{GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE},
	});

	glDebugMessageCallback(abd::gl::gl_debug_callback, nullptr);

	// VAO
	abd::gl::vao vao;
	glBindVertexArray(vao);

	// A square in a buffer
	abd::gl::gl_object<abd::gl::gl_object_type::BUFFER> buffer;
	float quad[] =
	{
		-1, -1, 0,
		1, -1, 0,
		-1, 1, 0, 

		1, -1, 0,
		1, 1, 0,
		-1, 1, 0
	};
	glNamedBufferStorage(buffer, sizeof(quad), &quad, GL_DYNAMIC_STORAGE_BIT);

	abd::gl::vao_attribute pos_attrib(vao.get_attribute(0));
	pos_attrib.set_format(3, GL_FLOAT, GL_FALSE, 0);
	pos_attrib.set_binding(0);

	vao.bind_buffer(0, buffer, 0, 3 * sizeof(float));


	// Shaders
	std::unique_ptr<abd::gl::program> prog_ptr;
	try
	{
		std::vector<abd::gl::shader> shaders;
		shaders.emplace_back(GL_VERTEX_SHADER,
			"#version 450 core\n"
			"layout (location = 0) in vec3 v_pos;\n"
			"void main(){\n"
			"	gl_Position = vec4(v_pos, 1.0);\n"
			"}\n");
		shaders.emplace_back(GL_FRAGMENT_SHADER,
			"#version 450 core\n"
			"out vec3 f_color;"
			"void main(){\n"
			"	f_color = vec3(1.0);\n"
			"}\n");
		prog_ptr = std::make_unique<abd::gl::program>(shaders);
	}
	catch (const abd::gl::shader_exception &ex)
	{
		auto trace = abd::trace_from_exception(ex);
		if (trace) std::cerr << "from " << trace.value() << std::endl;
		std::cerr << ex.what() << std::endl;
		std::cerr << ex.get_compile_log() << std::endl;
	}

	glUseProgram(*prog_ptr);


	win.run_main_loop([]()
	{
		glClearColor(0.1f, 0.1f, 0.1f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	});


	// GLEW destruction
	glfwTerminate();

	return 0;
}