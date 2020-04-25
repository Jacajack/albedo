#pragma once

#include <albedo/gl/shader.hpp>
#include <albedo/gl/program.hpp>
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
abd::gl::shader simple_load_shader(GLenum type, const boost::filesystem::path &path);


/**
	Reads all shaders in the directory and links them into one program.
	Shader type is derived from file extension:
		- .vs.glsl - GL_VERTEX_SHADER
		- .fs.glsl - GL_FRAGMENT_SHADER
		- .tcs.glsl - GL_TESS_CONTROL_SHADER
		- .tes.glsl - GL_TESS_EVALUATION_SHADER
		- .cs.glsl - GL_COMPUTE_SHADER
		- .gs.glsl - GL_GEOMETRY_SHADER
*/
abd::gl::program simple_load_shader_dir(const boost::filesystem::path &dir);


}