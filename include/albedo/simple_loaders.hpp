#pragma once

#include <albedo/gl/shader.hpp>
#include <albedo/mesh.hpp>
#include <boost/filesystem.hpp>

/**
	\file A buch of simple functional loaders.
*/

namespace abd {

/**
	Loads mesh data with assimp. All meshes in the Assimp scene are loaded into
	the compound mesh.
*/
abd::mesh_data assimp_simple_load_mesh(const boost::filesystem::path &path);

/**
	Slurps file and compiles it as a shader
*/
abd::gl::shader simple_load_shader(const boost::filesystem::path &path);


}