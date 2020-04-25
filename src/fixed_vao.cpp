#include <albedo/fixed_vao.hpp>
#include <albedo/mesh.hpp>

using abd::fixed_vao;

fixed_vao::fixed_vao(const vao_layout &conf) :
	m_layout(conf)
{
	// Init attributes
	if (m_layout.attrib_positions.has_value())
	{
		m_attrib_positions = this->get_attribute(m_layout.attrib_positions->bindingindex);
		m_attrib_positions->configure(m_layout.attrib_positions.value());
	}

	if (m_layout.attrib_normals.has_value())
	{
		m_attrib_normals = this->get_attribute(m_layout.attrib_normals->bindingindex);
		m_attrib_normals->configure(m_layout.attrib_normals.value());
	}

	if (m_layout.attrib_uvs.has_value())
	{
		m_attrib_uvs = this->get_attribute(m_layout.attrib_uvs->bindingindex);
		m_attrib_uvs->configure(m_layout.attrib_uvs.value());
	}
}

const abd::vao_layout &fixed_vao::get_layout() const
{
	return m_layout;
}