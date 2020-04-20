#pragma once

#include <albedo/gl/gl.hpp>
#include <albedo/gl/gl_object.hpp>
#include <albedo/utils.hpp>
#include <albedo/exception.hpp>
#include <string>

namespace abd {
namespace gl{

/**
	An exception caused by shader compilation error
*/
class shader_exception : public abd::exception
{
public:
	explicit shader_exception(const std::string &compile_log);
	explicit shader_exception(std::string &&compile_log);
	const std::string &get_compile_log() const
	{
		return m_compile_log;
	}

private:
	std::string m_compile_log;
};

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