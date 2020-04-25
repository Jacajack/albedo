#pragma once

#include <albedo/gl/gl.hpp>
#include <albedo/gl/gl_object.hpp>
#include <albedo/utils.hpp>
#include <albedo/exception.hpp>
#include <string>

namespace abd {
namespace gl{

//! A tag passed to shader_exception constructor
struct program_link_error {};

/**
	An exception caused by shader compilation error
*/
class shader_exception : public abd::exception
{
public:
	template <typename T>
	explicit shader_exception(T &&compile_log);

	template <typename T>
	explicit shader_exception(T &&link_log, program_link_error);

	const std::string &get_compile_log() const
	{
		return m_compile_log;
	}

private:
	std::string m_compile_log;
};

template <typename T>
shader_exception::shader_exception(T &&compile_log) :
	abd::exception("shader compilation error"),
	m_compile_log(std::forward<std::string>(compile_log))
{
}

template <typename T>
shader_exception::shader_exception(T &&link_log, program_link_error) :
	abd::exception("shader program link error"),
	m_compile_log(std::forward<std::string>(link_log))
{
}

/**
	A wrapper for OpenGL shader object.
*/
class shader : public abd::gl::gl_object<abd::gl::gl_object_type::SHADER>
{
public:
	shader(GLenum shader_type, const char *src);
	shader(GLenum shader_type, const std::string &src);

	std::string get_compile_log() const;
};


}
}