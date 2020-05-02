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
#include <albedo/gl/texture.hpp>
#include <albedo/gl/framebuffer.hpp>
#include <albedo/renderer.hpp>
#include <albedo/resource_manager.hpp>
#include <albedo/resource_defaults.hpp>

int main(int argc, char *argv[])
{
	// GLFW init
	glfwInit();

	// GLFW window init
	abd::gl::window win(1920, 1080, "test", {});

	glDebugMessageCallback(abd::gl::gl_debug_callback, nullptr);

	


	// Mesh manager
	auto eng = abd::main_resource_engine::get();
	auto &mesh_manager = eng.get_manager<abd::mesh>();
	auto monkey = mesh_manager.load("monkey.obj", "monkey");

	const abd::mesh_data &md = monkey->get_data();
	const abd::mesh_buffers &mb = monkey->get_buffers();


	// Deferred renderer
	abd::deferred_renderer renderer(1920, 1080);

	abd::draw_task_list dtl;
	dtl.mesh_draw_tasks.push_back({glm::mat4{1.0}, monkey});


	abd::perspective persp(
	    glm::radians(60.f),
	    1920.f / 1080.f,
	    0.1f,
	    200.f
	);

	abd::camera cam({0, 0, 5}, {0, 0, 0}, {0, 1, 0}, persp);

	auto &kh = win.get_keyboard_handler();
	auto is_pressed = [&kh](auto key) {return kh.get_key(key).is_pressed;};
	win.run_main_loop([&](double dt)
	{
		static double t = 0;
		t += dt;

		try
		{
			renderer.render_geometry(dtl, cam);
		}
		catch(const abd::exception& ex)
		{
			std::cerr << ex.get_trace() << std::endl;
		}

		float vel = 5;
		float vx = is_pressed(GLFW_KEY_D) - is_pressed(GLFW_KEY_A);
		float vz = is_pressed(GLFW_KEY_S) - is_pressed(GLFW_KEY_W);
		cam.relative_move({vx *dt * vel, 0, vz *dt * vel});

		// glClearColor(0.1f, 0.1f, 0.1f, 0.0f);
		// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// glEnable(GL_DEPTH_TEST);
		// glDrawElements(GL_TRIANGLES, md.indices.size(), GL_UNSIGNED_INT, 0);

		glBlitNamedFramebuffer(renderer.get_fbo(), 0, 0, 0, 1920, 1080, 0, 0, 1920, 1080, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	});

	

	// VAO
	// abd::fixed_vao vao{abd::standard_vao_layout};
	// vao.bind();



	// mesh_manager.remove("monkey");

	// mb.bind_to_vao(vao);

	

	// Shaders
	/*
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

	for (auto &[k, v] : prog_ptr->get_uniforms())
	{
		std::cout << "  unif - " << k << std::endl;
	}

	for (auto &[k, v] : prog_ptr->get_uniform_blocks())
	{
		std::cout << "  block - " << k << std::endl;
	}

	GLint ul_mvp = prog_ptr->get_uniform("m_mvp").get_location();
	// GLint ul_test = prog_ptr->get_uniform("TEST_BLOCK.color").get_location();

	std::vector<glm::vec3> v_test{{1, 0.4, 1}};
	auto buf_test = abd::gl::vector_to_buffer(v_test, GL_DYNAMIC_STORAGE_BIT);
	GLuint bi = glGetUniformBlockIndex(*prog_ptr, "TEST_BLOCK");
	glUniformBlockBinding(*prog_ptr, bi, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, *buf_test);
	*/

	
	


	// Create a framebuffer and textures
	/*
	abd::gl::texture<abd::gl::texture_target::TEXTURE_2D> tex_depth, tex_color;
	tex_depth.storage_2d(abd::gl::texture_format::DEPTH_32F, 1920, 1080);
	tex_depth.set_min_filter(GL_LINEAR);
	tex_depth.set_mag_filter(GL_LINEAR);
	tex_color.storage_2d(abd::gl::texture_format::RGB8, 1920, 1080);
	tex_color.set_min_filter(GL_LINEAR);
	tex_color.set_mag_filter(GL_LINEAR);
	abd::gl::framebuffer fbo;
	fbo.attach_texture(GL_DEPTH_ATTACHMENT, tex_depth);
	fbo.attach_texture(GL_COLOR_ATTACHMENT0, tex_color);
	fbo.set_draw_buffers({GL_COLOR_ATTACHMENT0});
	std::cerr << abd::gl::enum_to_str(fbo.get_status()) << std::endl;
	if (!fbo.is_complete()) throw std::runtime_error("FBO not complete :c");
	*/


	/*
	mb.bind_index_buffer();

	auto &kh = win.get_keyboard_handler();
	auto &mh = win.get_mouse_handler();
	auto is_pressed = [&kh](auto key) {return kh.get_key(key).is_pressed;};
	//[&win](auto key){return glfwGetKey(win.get(), key) == GLFW_PRESS;};

	win.run_main_loop([&](double dt)
	{
		static double t = 0;
		t += dt;
		glUniformMatrix4fv(ul_mvp, 1, GL_FALSE, &cam.get_matrix()[0][0]);

		float vel = 5;
		float vx = is_pressed(GLFW_KEY_D) - is_pressed(GLFW_KEY_A);
		float vz = is_pressed(GLFW_KEY_S) - is_pressed(GLFW_KEY_W);
		cam.relative_move({vx *dt * vel, 0, vz *dt * vel});
		// cam.look_at({0, 0, 0});

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glViewport(0, 0, 1920, 1080);
		glClearColor(0.1f, 0.1f, 0.1f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glDrawArrays(GL_TRIANGLES, 0, 6);
		glEnable(GL_DEPTH_TEST);
		glDrawElements(GL_TRIANGLES, md.indices.size(), GL_UNSIGNED_INT, 0);

		glBlitNamedFramebuffer(fbo, 0, 0, 0, 1920, 1080, 0, 0, 1920, 1080, GL_COLOR_BUFFER_BIT, GL_LINEAR);

		// std::cout << mh.get_position().x << std::endl;
	});
	*/


	// GLEW destruction
	glfwTerminate();

	return 0;
}