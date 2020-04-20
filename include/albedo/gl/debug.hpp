#pragma once

#include <albedo/gl/gl.hpp>
#include <albedo/utils.hpp>
#include <string>

namespace abd {
namespace gl {

/**
	The global OpenGL debug message handler
*/
void APIENTRY gl_debug_callback(GLenum source,
								GLenum type,
								GLuint id,
								GLenum severity,
								GLsizei length,
								const GLchar *message,
								void *user_param);

/**
	A RAII wrapper for OpenGL debug group.
	A new debug group is pushed on creation
	and removed upon destruction.
*/
class debug_group_object : abd::noncopy
{
public:
	debug_group_object(GLuint id, const std::string &message);
	debug_group_object(GLuint id, const char *message);
	~debug_group_object();	

	debug_group_object(debug_group_object &&) = delete;
	debug_group_object &operator=(debug_group_object &&) = delete;
};

}
}