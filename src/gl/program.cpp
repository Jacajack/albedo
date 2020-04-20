#include <albedo/gl/program.hpp>

using abd::gl::program;

std::string program::get_link_log() const
{
	GLint length = this->get_parameter<GLint>(GL_INFO_LOG_LENGTH);
	if (length > 0)
	{
		char arr[length + 1];
		glGetProgramInfoLog(*this, length, NULL, arr);
		return std::string{arr};
	}
	else
		return {};
}
