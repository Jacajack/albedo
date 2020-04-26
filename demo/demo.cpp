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
#include <albedo/camera.hpp>

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
	std::cout << md.normals.size() << " normals" << std::endl;
	abd::mesh_buffers mb(md);
	mb.bind_to_vao(vao);
	//vao.bind_buffer(0, *mb.m_position_buffer, 0, 3 * sizeof(float));


	// Shaders
	std::unique_ptr<abd::gl::program> prog_ptr;
	try
	{
		prog_ptr = std::make_unique<abd::gl::program>(abd::simple_load_shader_dir("test_shader"));
	}
	catch (const abd::gl::shader_exception &ex)
	{
		auto trace = abd::trace_from_exception(ex);
		if (trace) std::cerr << "from " << trace.value() << std::endl;
		std::cerr << ex.what() << std::endl;
		std::cerr << ex.get_compile_log() << std::endl;
	}

	prog_ptr->use();

	for (auto &[k,v] : prog_ptr->get_uniforms())
	{
		std::cout << "  unif - " << k << std::endl;
	}

	for (auto &[k,v] : prog_ptr->get_uniform_blocks())
	{
		std::cout << "  block - " << k << std::endl;
	}

	GLint ul_mvp = prog_ptr->get_uniform("m_mvp").get_location();
	std::cout << "m_mvp at " << ul_mvp << " " << glGetUniformLocation(prog_ptr->id(), "m_mvp") << std::endl;

	GLint ul_test = prog_ptr->get_uniform("TEST_BLOCK.color").get_location();
	std::cout << "test at " << ul_test << " " << glGetUniformLocation(prog_ptr->id(), "TEST_BLOCK.color") << std::endl;

	std::vector<glm::vec3> v_test{{1, 0.4, 1}};
	auto buf_test = abd::gl::vector_to_buffer(v_test, GL_DYNAMIC_STORAGE_BIT);
	GLuint bi = glGetUniformBlockIndex(*prog_ptr, "TEST_BLOCK");
	glUniformBlockBinding(*prog_ptr, bi, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, *buf_test);

	//glUniform3f(ul_test, 1, 0, 1);

	abd::perspective persp(
		glm::radians(60.f),
		640.f / 480.f,
		0.1f,
		200.f
	);

	abd::camera cam({0, 0, -5}, {0, 0, 0}, {0, 1, 0}, persp);

	

	mb.bind_index_buffer();
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *mb.m_index_buffer);

	win.run_main_loop([&](double dt)
	{
		static double t = 0;
		t += dt;
		glUniformMatrix4fv(ul_mvp, 1, GL_FALSE, &cam.get_matrix()[0][0]);

		float cam_r = 5;
		cam.set_position({sin(t) * cam_r, 0, cos(t) * cam_r});
		cam.look_at({0, 0, 0});

		glClearColor(0.1f, 0.1f, 0.1f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glDrawArrays(GL_TRIANGLES, 0, 6);
		glEnable(GL_DEPTH_TEST);
		glDrawElements(GL_TRIANGLES, md.indices.size(), GL_UNSIGNED_INT, 0);
	});


	// GLEW destruction
	glfwTerminate();

	return 0;
}