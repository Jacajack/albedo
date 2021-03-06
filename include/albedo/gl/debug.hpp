#pragma once

#include <albedo/gl/gl.hpp>
#include <albedo/utils.hpp>
#include <string>

namespace abd::gl {

/**
	The global OpenGL debug message handler
*/
void APIENTRY gl_debug_callback(GLenum source,
								GLenum type,
								GLuint id,
								GLenum severity,
								GLsizei length,
								const GLchar *message,
								const void *user_param);

/**
	A RAII wrapper for OpenGL debug group.
	A new debug group is pushed on creation
	and removed upon destruction.
*/
class debug_group : abd::noncopy
{
public:
	debug_group(GLuint id, const std::string &message);
	debug_group(GLuint id, const char *message);
	~debug_group();	

	debug_group(const debug_group &) = delete;
	debug_group &operator=(const debug_group &) = delete;

	debug_group(debug_group &&) = delete;
	debug_group &operator=(debug_group &&) = delete;
};

/**
	Converts a GLenum value to a corresponding string
*/
const char *glenum_to_str(GLenum val);

} // namespace abd::gl