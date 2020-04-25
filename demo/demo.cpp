#include <iostream>
#include <albedo/albedo.hpp>
#include <albedo/gl/window.hpp>
#include <albedo/gl/gl_object.hpp>
#include <albedo/gl/program.hpp>
#include <albedo/gl/debug.hpp>
#include <albedo/gl/vertex_array.hpp>
#include <albedo/mesh.hpp>
#include <albedo/simple_loaders.hpp>
#include <memory>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

void foo()
{
	throw abd::traced_exception<std::runtime_error>("haha");
}

int main(int argc, char *argv[])
{
	// GLFW init
	assert(glfwInit() == GLFW_TRUE);

	// GLFW window init
	abd::gl::window win(640, 480, "test", {});

	glDebugMessageCallback(abd::gl::gl_debug_callback, nullptr);

	// VAO
	abd::fixed_vao vao{abd::standard_vao_layout};
	vao.bind();

	// A square in a buffer
	float quad[] =
	{
		-1, -1, 0,
		1, -1, 0,
		-1, 1, 0, 

		1, -1, 0,
		1, 1, 0,
		-1, 1, 0
	};
	abd::gl::buffer buffer(sizeof(quad), quad, GL_DYNAMIC_STORAGE_BIT);

	//auto pos_attrib{vao.get_attribute(0)};
	//pos_attrib.set_format(3, GL_FLOAT, GL_FALSE, 0);
	//pos_attrib.set_binding(0);

	//vao.bind_buffer(0, buffer, 0, 3 * sizeof(float));

	abd::mesh_data md = abd::assimp_simple_load_mesh("monkey.obj");
	std::cout << md.indices.size() << " indices" << std::endl;
	abd::mesh_buffers mb(md);
	mb.bind_to_vao(vao);
	//vao.bind_buffer(0, *mb.m_position_buffer, 0, 3 * sizeof(float));


	// Shaders
	std::unique_ptr<abd::gl::program> prog_ptr;
	try
	{
		std::vector<abd::gl::shader> shaders;
		shaders.emplace_back(GL_VERTEX_SHADER,
			"#version 450 core\n"
			"layout (location = 0) in vec3 v_pos;\n"
			"uniform mat4 m_mvp;"
			"void main(){\n"
			"	gl_Position = m_mvp * vec4(v_pos, 1.0);\n"
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

	prog_ptr->use();

	GLuint ul_mvp = glGetUniformLocation(*prog_ptr, "m_mvp");
	std::cout << "m_mvp at " << ul_mvp << std::endl;


	glm::mat4 mat_view = glm::lookAt(
		glm::vec3(0, 0, -5),
		glm::vec3(0, 0, 0),
		glm::vec3(0, 1, 0)
	);

	glm::mat4 mat_proj = glm::perspective(
		glm::radians(60.f),
		640.f / 480.f,
		0.1f,
		200.f
	);

	glm::mat4 mat_mvp = mat_proj * mat_view;

	glUniformMatrix4fv(ul_mvp, 1, GL_FALSE, &mat_mvp[0][0]);

	mb.bind_index_buffer();
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *mb.m_index_buffer);

	win.run_main_loop([&]()
	{
		glClearColor(0.1f, 0.1f, 0.1f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glDrawArrays(GL_TRIANGLES, 0, 6);
		glDrawElements(GL_TRIANGLES, md.indices.size(), GL_UNSIGNED_INT, 0);
	});


	// GLEW destruction
	glfwTerminate();

	return 0;
}