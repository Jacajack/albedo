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

/**
	Binds a set of buffers to a VAO with fixed layout.

	For simplcity it is assumed that buffers have predefined layouts.
	Fancy stuff such as interleaving will be supported later.
*/
void mesh_buffers::bind_to_vao(abd::fixed_vao &vao) const
{
	auto layout = vao.get_layout();

	if (layout.attrib_positions.has_value() && this->m_position_buffer)
		vao.bind_buffer(layout.attrib_positions->bindingindex, *m_position_buffer, 0, 3 * sizeof(float));

	if (layout.attrib_normals.has_value() && this->m_normal_buffer)
		vao.bind_buffer(layout.attrib_normals->bindingindex, *m_normal_buffer, 0, 3 * sizeof(float));

	if (layout.attrib_uvs.has_value() && this->m_uv_buffer)
		vao.bind_buffer(layout.attrib_uvs->bindingindex, *m_uv_buffer, 0, 2 * sizeof(float));
}

/**
	Binds the index buffer to GL_ELEMENT_ARRAY_BUFFER target
*/
void mesh_buffers::bind_index_buffer() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *m_index_buffer);
}