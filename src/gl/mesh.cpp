#include <albedo/mesh.hpp>

using abd::mesh_data;
using abd::mesh_buffers;

template <typename T>
static std::unique_ptr<abd::gl::buffer> vector_to_buffer(const std::vector<T> &v, GLbitfield flags)
{
	return std::make_unique<abd::gl::buffer>(v.size() * sizeof(T), v.data(), flags);
}

/**
	Buffers data provided in the 
*/
mesh_buffers::mesh_buffers(const mesh_data &data)
{
	const GLbitfield flags = GL_DYNAMIC_STORAGE_BIT;
	
	if (data.indices.size()) m_index_buffer = vector_to_buffer(data.indices, flags);
	if (data.vertices.size()) m_vertex_buffer = vector_to_buffer(data.vertices, flags);
	if (data.normals.size()) m_normal_buffer = vector_to_buffer(data.normals, flags);
	if (data.uvs.size()) m_uv_buffer = vector_to_buffer(data.uvs, flags);			
}