#include <albedo/renderer.hpp>
#include <albedo/exception.hpp>
#include <albedo/gl/program.hpp>
#include <albedo/simple_loaders.hpp>
#include <albedo/gl/debug.hpp>
#include <iostream>
#include <array>
#include <future>

using abd::deferred_renderer;

bool abd::light_draw_task::operator<(const abd::light_draw_task &rhs) const
{
	// Sort by volume type
	if (this->volume == rhs.volume)
	{
		// Sort by mesh if different
		if (this->volume == light_volume_type::MESH && this->volume_mesh_ptr != rhs.volume_mesh_ptr)
			return this->volume_mesh_ptr < rhs.volume_mesh_ptr;
		
		// Sort identical meshes by light type
		return this->type < rhs.type;
	}
	else return this->volume < rhs.volume;
}

deferred_renderer::deferred_renderer(int width, int height) :
	m_blit_quad(6 * 3 * sizeof(float), nullptr, GL_DYNAMIC_STORAGE_BIT),
	m_lights_buffer(max_light_count * sizeof(ubo_light_data), GL_MAP_WRITE_BIT),
	m_fbo_width(width),
	m_fbo_height(height)
{
	// Load shaders
	try
	{
		m_geometry_program = std::make_unique<gl::program>(abd::simple_load_shader_dir("albedo/deferred/geometry_pass"));
		m_shading_program = std::make_unique<gl::program>(abd::simple_load_shader_dir("albedo/deferred/shading"));
		m_blit_program = std::make_unique<gl::program>(abd::simple_load_shader_dir("albedo/deferred/blit"));
	}
	catch (const abd::gl::shader_exception &ex)
	{
		std::cerr << ex.what() << std::endl;
		std::cerr << ex.get_compile_log() << std::endl;
		throw abd::exception("deferred_renderer could not compile essential shaders");
	}
	catch (const std::exception &ex)
	{
		std::cerr << ex.what() << std::endl;
		throw abd::exception("deferred_renderer could not load essential shaders");
	}

	// The blit quad
	std::array<float, 18> quad_data =
	{
		-1, -1, 0,
		1, -1, 0,
		-1, 1, 0,
		
		-1, 1, 0,
		1, -1, 0,
		1, 1, 0,
	};
	m_blit_quad.write(0, quad_data.size() * sizeof(float), quad_data.data());

	// Create depth texture
	m_gbuffer.depth.storage_2d(gl::texture_format::DEPTH_32F, width, height);
	m_gbuffer.depth.set_min_filter(GL_LINEAR);
	m_gbuffer.depth.set_mag_filter(GL_LINEAR);
	m_fbo.attach_texture(GL_DEPTH_ATTACHMENT,m_gbuffer.depth);

	// Create color texture
	m_color_buffer.storage_2d(gl::texture_format::RGB16F, width, height);
	m_color_buffer.set_min_filter(GL_LINEAR);
	m_color_buffer.set_mag_filter(GL_LINEAR);
	m_fbo.attach_texture(GL_COLOR_ATTACHMENT0, m_color_buffer);

	// Create position texture
	m_gbuffer.position.storage_2d(gl::texture_format::RGB32F, width, height);
	m_gbuffer.position.set_min_filter(GL_LINEAR);
	m_gbuffer.position.set_mag_filter(GL_LINEAR);
	m_fbo.attach_texture(GL_COLOR_ATTACHMENT1, m_gbuffer.position);

	// Normal texture
	m_gbuffer.normal.storage_2d(gl::texture_format::RGB16F, width, height);
	m_gbuffer.normal.set_min_filter(GL_LINEAR);
	m_gbuffer.normal.set_mag_filter(GL_LINEAR);
	m_fbo.attach_texture(GL_COLOR_ATTACHMENT2, m_gbuffer.normal);

	// Diffuse color buffer
	m_gbuffer.diffuse.storage_2d(gl::texture_format::RGB8, width, height);
	m_gbuffer.diffuse.set_min_filter(GL_LINEAR);
	m_gbuffer.diffuse.set_mag_filter(GL_LINEAR);
	m_fbo.attach_texture(GL_COLOR_ATTACHMENT3, m_gbuffer.diffuse);

	// Specular info buffer
	m_gbuffer.specular.storage_2d(gl::texture_format::RGB8, width, height);
	m_gbuffer.specular.set_min_filter(GL_LINEAR);
	m_gbuffer.specular.set_mag_filter(GL_LINEAR);
	m_fbo.attach_texture(GL_COLOR_ATTACHMENT4, m_gbuffer.specular);

	if (!m_fbo.is_complete())
		throw abd::exception("deferred_renderer's FBO is incomplete!");
}


void deferred_renderer::render(abd::draw_task_list draw_tasks, const abd::camera &camera, GLuint output_fbo)
{
	// Prepare lighting data while the geometry is rendered
	auto lights_buffer_chunk = m_lights_buffer.get_chunk();
	auto lights_data_ready = std::async([this, &draw_tasks, &lights_buffer_chunk]()
	{
		this->prepare_lights_data(draw_tasks.light_draw_tasks, lights_buffer_chunk);
	});

	// Start the geometry pass
	geometry_pass(draw_tasks.mesh_draw_tasks, camera);

	// Wait for lighting data to be processed and initiate lighting pass
	lights_data_ready.wait();
	lighting_pass(draw_tasks.light_draw_tasks, lights_buffer_chunk, camera);

	//! \todo postprocessing here
	blit_to_output(output_fbo);
}

/**
	Prepares light data in the light's UBO asynchronously while the geometry is being processed.
*/
void deferred_renderer::prepare_lights_data(std::vector<light_draw_task> &light_tasks, gl::synced_buffer_handle &lights_buffer_chunk)
{
	auto *lights_data = static_cast<ubo_light_data*>(lights_buffer_chunk.get_ptr());

	// Sort lights in the processing order
	std::sort(light_tasks.begin(), light_tasks.end());
	if (light_tasks.size() > max_light_count)
		throw abd::exception("too many light draw tasks passed to the renderer");

	for (unsigned int i = 0; i < light_tasks.size(); i++)
	{
		auto &data = lights_data[i];
		auto &task = light_tasks[i];
		
		data.light_type        = static_cast<GLint>(task.type);
		data.blend             = task.blend;
		data.color_specular    = glm::vec4{task.color * task.power, task.specular};
		data.position_distance = glm::vec4{task.position, task.distance};
		data.direction_angle   = glm::vec4{glm::normalize(task.direction), task.angle};
	}
}


void deferred_renderer::geometry_pass(std::vector<mesh_draw_task> &mesh_tasks, const abd::camera &camera)
{
	abd::gl::debug_group d(0, "abd::deferred_renderer geometry pass");

	// Beginning of the geometry pass - bind MRT
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
	m_fbo.set_draw_buffers({
		GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1,
		GL_COLOR_ATTACHMENT2,
		GL_COLOR_ATTACHMENT3,
		GL_COLOR_ATTACHMENT4,
	});

	// Use the main VAO and geometry pass program
	m_vao.bind();
	m_geometry_program->use();

	// Clear buffers, enable depth test and disable blending
	glClearColor(0, 0, 0, 0);
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDisable(GL_BLEND);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Get uniform locations (TODO this should only be done once)
	auto &uni_mat_model = m_geometry_program->get_uniform("mat_model");
	auto &uni_mat_view = m_geometry_program->get_uniform("mat_view");
	auto &uni_mat_proj = m_geometry_program->get_uniform("mat_proj");
	auto &uni_mat_vp = m_geometry_program->get_uniform("mat_vp");
	auto &uni_mat_mvp = m_geometry_program->get_uniform("mat_mvp");

	// Pass view and projection matrices to the shader
	uni_mat_view = camera.get_view_matrix();
	uni_mat_proj = camera.get_projection_matrix();
	uni_mat_vp = camera;

	//! \todo sort mesh draw_tasks to minimize context-changes
	// Execute every draw task
	for (const auto &task : mesh_tasks)
	{
		auto &mesh = *task.mesh_ptr;
		auto &mesh_data = mesh.get_data();
		auto &mesh_buffers = mesh.get_buffers();

		mesh_buffers.bind_index_buffer();
		mesh_buffers.bind_to_vao(m_vao);

		// Update model matrix
		uni_mat_model = task.transform;
		uni_mat_mvp = camera.get_matrix() * task.transform;

		//! \todo replace with glMutliDraw*()
		// Draw all sub-meshes one by one
		for (unsigned int i = 0; i < mesh_data.base_indices.size(); i++)
		{
			glDrawElementsBaseVertex(
				GL_TRIANGLES,
				mesh_data.draw_sizes[i],
				GL_UNSIGNED_INT,            //! \todo this should be based on type provided by abd::mesh
				nullptr,
				mesh_data.base_indices[i]
				);
		}
	}
}


void deferred_renderer::lighting_pass(std::vector<light_draw_task> &light_tasks, gl::synced_buffer_handle &lights_buffer_chunk, const abd::camera &camera)
{
	abd::gl::debug_group d_shad(1, "abd::deferred_renderer shading pass");

	// Attach the VAO
	m_vao.bind();
	
	// Attach only color buffer and clear it
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
	m_fbo.set_draw_buffers({GL_COLOR_ATTACHMENT0});

	// Use shading program and bind G-buffer textures (standard layout)
	//! \todo Uniform locations should only be retrieved once!
	m_shading_program->use();
	glBindTextureUnit(1, m_gbuffer.position);
	glBindTextureUnit(2, m_gbuffer.normal);
	glBindTextureUnit(3, m_gbuffer.diffuse);
	glBindTextureUnit(4, m_gbuffer.specular);
	m_shading_program->get_uniform("tex_position") = 1;
	m_shading_program->get_uniform("tex_normal")   = 2;
	m_shading_program->get_uniform("tex_diffuse")  = 3;
	m_shading_program->get_uniform("tex_specular") = 4;

	// Matrices
	m_shading_program->get_uniform("mat_view") = camera.get_view_matrix();
	m_shading_program->get_uniform("mat_proj") = camera.get_projection_matrix();
	m_shading_program->get_uniform("mat_vp")   = camera.get_matrix();

	// Additive blending
	glBlendFunc(GL_SRC_COLOR, GL_DST_COLOR);
	glBlendEquation(GL_FUNC_ADD);
	glEnable(GL_BLEND);

	// Bind the UBO to the program (at binding 0)
	GLuint light_ub_id = glGetUniformBlockIndex(*m_shading_program, "LIGHTS_UBO");
	if (light_ub_id == GL_INVALID_INDEX)
		throw abd::exception("could not access UBO containing light data");
	glUniformBlockBinding(*m_shading_program, light_ub_id, 0);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, lights_buffer_chunk.get_buffer(), lights_buffer_chunk.get_offset(), lights_buffer_chunk.get_size());

	// Count global lights
	int global_light_count{0};
	for (const auto &task : light_tasks)
		if (task.volume == light_draw_task::light_volume_type::GLOBAL)
			global_light_count++;
		else
			break;

	// Process global lights (if any)
	if (global_light_count > 0)
	{
		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);

		m_vao.bind_buffer(0, m_blit_quad, {0, 3 * sizeof(float)});
		m_shading_program->get_uniform("base_light_index") = 0;
		m_shading_program->get_uniform("light_count") = global_light_count;
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}


	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);

	//! \todo Light volume processing here

	lights_buffer_chunk.flush();
	lights_buffer_chunk.fence();
}

void deferred_renderer::blit_to_output(GLuint output_fbo)
{
	// Blit color buffer to the output FBO
	// Attach the blit quad to the VAO, disable depth test and blending
	m_vao.bind_buffer(0, m_blit_quad, {0, 3 * sizeof(float)});
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glDisable(GL_BLEND);
	m_blit_program->use();
	m_blit_program->get_uniform("input_tex") = 0;
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, output_fbo);
	glBindTextureUnit(0, m_color_buffer);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}
