#include <albedo/gl/debug.hpp>
#include <unordered_map>
#include <iostream>

using abd::gl::debug_group_object;

void APIENTRY gl_debug_callback(GLenum source,
                                GLenum type,
                                GLuint id,
                                GLenum severity,
                                GLsizei length,
                                const GLchar *message_cstr,
                                void *user_param)
{
	static const std::unordered_map<GLenum, std::string> source_names
	{
		{GL_DEBUG_SOURCE_API, "API"},
		{GL_DEBUG_SOURCE_SHADER_COMPILER, "shader compiler"},
		{GL_DEBUG_SOURCE_WINDOW_SYSTEM, "window system"},
		{GL_DEBUG_SOURCE_THIRD_PARTY, "third party"},
		{GL_DEBUG_SOURCE_APPLICATION, "application"},
		{GL_DEBUG_SOURCE_OTHER, "other"},
	};

	static const std::unordered_map<GLenum, std::string> type_names
	{
		{GL_DEBUG_TYPE_ERROR, "error"},
		{GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR, "deprecated behavior"},
		{GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR, "undefined behavior"},
		{GL_DEBUG_TYPE_PERFORMANCE, "performance issue"},
		{GL_DEBUG_TYPE_PORTABILITY, "portability issue"},
		{GL_DEBUG_TYPE_MARKER, "marker"},
		{GL_DEBUG_TYPE_PUSH_GROUP, "group push"},
		{GL_DEBUG_TYPE_POP_GROUP, "group pop"},
		{GL_DEBUG_TYPE_OTHER, "other"},
	};

	static const std::unordered_map<GLenum, std::string> severity_names
	{
		{GL_DEBUG_SEVERITY_LOW, "low"},
		{GL_DEBUG_SEVERITY_MEDIUM, "medium"},
		{GL_DEBUG_SEVERITY_HIGH, "high"},
		{GL_DEBUG_SEVERITY_NOTIFICATION, "notification"},
	};

	std::string message = length > 0 ? std::string{message_cstr, message_cstr + length} : std::string{message_cstr};

	//! \todo replace this call with some proper handler object
	std::cerr << type_names.at(type) << " reported by " << source_names.at(source) << ": " << message << std::endl;
}



debug_group_object::debug_group_object(GLuint id, const std::string &message)
	: debug_group_object(id, message.c_str())
{
}

debug_group_object::debug_group_object(GLuint id, const char *message)
{
	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, id, 0, message);
}

debug_group_object::~debug_group_object()
{
	glPopDebugGroup();
}
