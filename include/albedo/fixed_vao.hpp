#pragma once

#include <albedo/gl/vertex_array.hpp>
#include <optional>

namespace abd {

/**
	List of optional attribute configurations
*/
struct vao_layout
{
	std::optional<gl::vao_attribute_config> attrib_positions;
	std::optional<gl::vao_attribute_config> attrib_normals;
	std::optional<gl::vao_attribute_config> attrib_uvs;
};

/**
	Represents a VAO with fixed attribute setup.
*/
class fixed_vao : public gl::vao
{
public:
	fixed_vao(const vao_layout &conf);

	const vao_layout &get_layout() const;

private:
	using gl::vao::get_attribute;

	vao_layout m_layout;
	std::optional<gl::vao_attribute> m_attrib_positions;
	std::optional<gl::vao_attribute> m_attrib_normals;
	std::optional<gl::vao_attribute> m_attrib_uvs;
};


/**
	Standard VAO layout for forward rendering. Requires
	3 separate buffers for positions, normals and UVs.
		- attr 0 - vec3 positions
		- attr 1 - vec3 normals
		- attr 2 - vec2 uvs
*/
inline const vao_layout standard_vao_layout = 
{
	// C++20
	.attrib_positions = {{0, 3, GL_FLOAT, GL_FALSE, 0}},
	.attrib_normals = {{1, 3, GL_FLOAT, GL_FALSE, 0}},
	.attrib_uvs = {{2, 2, GL_FLOAT, GL_FALSE, 0}},
};


}