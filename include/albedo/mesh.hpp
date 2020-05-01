#pragma once

#include <albedo/gl/gl.hpp>
#include <albedo/exception.hpp>
#include <albedo/gl/buffer.hpp>
#include <albedo/gl/vertex_array.hpp>
#include <albedo/material.hpp>
#include <albedo/fixed_vao.hpp>
#include <albedo/resource.hpp>
#include <vector>

namespace abd {


/**
	Represents any (simple or compound) mesh data.
	When loaded into GPU, the meshes are loaded into one
	set of buffers.

	\todo add materials vector
*/
struct mesh_data
{
	std::vector<GLint> base_indices;
	std::vector<GLint> draw_sizes;
	std::vector<material> materials;

	std::vector<GLuint> indices;
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;
};

/**
	Contains OpenGL buffers with mesh data.
	As long as this object exists, the data is buffered in the GPU.
*/
class mesh_buffers
{
	friend class fixed_vao;

public:
	mesh_buffers(const mesh_data &data);
	
	void bind_to_vao(fixed_vao &vao) const;
	void bind_index_buffer() const;

private:
	GLenum m_index_data_type;
	std::unique_ptr<abd::gl::buffer> m_index_buffer;
	std::unique_ptr<abd::gl::buffer> m_position_buffer;
	std::unique_ptr<abd::gl::buffer> m_normal_buffer;
	std::unique_ptr<abd::gl::buffer> m_uv_buffer;
};


/**
	Owns mesh_data and mesh_buffers.
	Guarantees that mesh_buffers are populated with the data from mesh_data
*/
class mesh : public abd::resource<mesh>
{
public:
	template <typename T, typename = std::enable_if<std::is_same_v<std::decay_t<T>, mesh_data>>>
	mesh(T &&data) :
		m_data(std::forward<mesh_data>(data)),
		m_buffers(std::make_unique<mesh_buffers>(m_data))
	{
	}

	const mesh_data &get_data() const
	{
		return m_data;
	}

	const mesh_buffers &get_buffers() const
	{
		return *m_buffers;
	}

private:
	mesh_data m_data;
	std::unique_ptr<mesh_buffers> m_buffers;
};


}