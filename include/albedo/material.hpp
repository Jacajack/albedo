#pragma once

#include <albedo/gl/gl.hpp>

namespace abd {

/**
	Basic material properties
*/
struct material_data
{
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
	float reflective;
};

/**
	Temporary thing, just in case if another
	wrapper appears
*/
using material = material_data;

}