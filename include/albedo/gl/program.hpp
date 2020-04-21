#pragma once

#include <albedo/gl/gl.hpp>
#include <albedo/gl/gl_object.hpp>
#include <albedo/gl/shader.hpp>
#include <initializer_list>
#include <string>

namespace abd {
namespace gl {

/**
	Represents RAII wrapper for an OpenGL program object
*/
class program : public abd::gl::gl_object<abd::gl::gl_object_type::PROGRAM>
{
public:
	template <typename Tcont>
	explicit program(const Tcont &shaders);

	std::string get_link_log() const;

	void use() const
	{
		glUseProgram(*this);
	}
};

/**
	This constructor allows program creation based on any container
	of shaders
*/
template <typename Tcont>
program::program(const Tcont &shaders)
{
	// Attach all shaders
	for (const auto &s : shaders)
		glAttachShader(*this, s);

	// Link the program
	glLinkProgram(*this);

	// Detach all shaders
	for (const auto &s : shaders)
		glDetachShader(*this, s);

	// Check result
	if (this->get_parameter<GLint>(GL_LINK_STATUS) == GL_FALSE)
	{
		throw abd::gl::shader_exception(get_link_log());
	}
}


}
}