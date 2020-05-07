#pragma once

#include <albedo/gl/gl.hpp>
#include <albedo/texture.hpp>
#include <albedo/gl/program.hpp>

namespace abd {

/**
	Basic material properties
*/
struct material_data
{
	//! Determines diffuse reflection color
	glm::vec3 diffuse;
	std::weak_ptr<abd::texture_2d> tex_diffuse;

	// Specular characteristics
	float specular;
	float roughness;
	float specular_tint;
};

/**
	\todo THIS
*/
class material
{

public:
	std::uint64_t id() const;

	material(const material_data &d) : m_data(d) {}

	material_data &get_data()
	{
		return m_data;
	}

private:
	static std::uint64_t id_counter;

	material_data m_data;
	std::uint64_t m_id;
};

}
