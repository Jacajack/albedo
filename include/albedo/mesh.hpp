#pragma once

#include <albedo/gl/gl.hpp>
#include <albedo/exception.hpp>
#include <albedo/gl/buffer.hpp>
#include <albedo/gl/vertex_array.hpp>
#include <albedo/material.hpp>
#include <albedo/fixed_vao.hpp>
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
	Represents a compund mesh. Contains owning pointer to mesh_buffers class.
*/
class mesh
{

private:
	mesh_data m_data;
	std::unique_ptr<mesh_buffers> m_buffers;
};




}