#include <albedo/gl/shader.hpp>

using abd::gl::shader;

shader::shader(GLenum shader_type, const std::string &src) :
	shader(shader_type, src.c_str())
{
}

shader::shader(GLenum shader_type, const char *src) :
	gl_object<abd::gl::gl_object_type::SHADER>(shader_type)
{
	// Provide source code
	glShaderSource(*this, 1, &src, nullptr);

	// Compile the shader
	glCompileShader(*this);

	// Throw compilation exception if failed to compile
	if (this->get_parameter<GLint>(GL_COMPILE_STATUS) == GL_FALSE)
	{
		throw abd::gl::shader_exception(this->get_compile_log());
	}
}

std::string shader::get_compile_log() const
{
	GLint length = this->get_parameter<GLint>(GL_INFO_LOG_LENGTH);
	if (length > 0)
	{
		char arr[length + 1];
		glGetShaderInfoLog(*this, length, NULL, arr);
		return std::string{arr};
	}
	else
		return {};
}