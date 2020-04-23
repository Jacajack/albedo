#include <albedo/mesh.hpp>

using abd::mesh_data;
using abd::mesh_buffers;

/**
	Buffers data provided in the mesh_data or compound_mesh_data in GPU.
*/
mesh_buffers::mesh_buffers(const mesh_data &data)
{
	const GLbitfield flags = GL_DYNAMIC_STORAGE_BIT;

	// Do not allow any mesh that is incomplete
	if (!data.indices.size() || !data.positions.size() || !data.normals.size())
		throw abd::exception("cannot greate mesh_buffers from icomplete mesh data");

	m_index_buffer = abd::gl::vector_to_buffer(data.indices, flags);
	m_position_buffer = abd::gl::vector_to_buffer(data.positions, flags);
	m_normal_buffer = abd::gl::vector_to_buffer(data.normals, flags);
	if (!data.uvs.empty()) m_uv_buffer = abd::gl::vector_to_buffer(data.uvs, flags);			
}