#pragma once

#include <albedo/gl/gl.hpp>
#include <albedo/exception.hpp>
#include <albedo/gl/buffer.hpp>
#include <albedo/material.hpp>
#include <vector>

namespace abd {

/**
	Contains mesh data - stored in RAM memory
*/
struct mesh_data
{
	abd::material material;
	std::vector<GLint> indices;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;
};

/**
	Contains OpenGL buffers with mesh data.
	As long as this object exists, the data is buffered in the GPU.
*/
class mesh_buffers
{
public:
	mesh_buffers(const mesh_data &data);

private:
	std::unique_ptr<abd::gl::buffer> m_index_buffer;
	std::unique_ptr<abd::gl::buffer> m_vertex_buffer;
	std::unique_ptr<abd::gl::buffer> m_normal_buffer;
	std::unique_ptr<abd::gl::buffer> m_uv_buffer;
};

}