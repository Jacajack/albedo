#include <albedo/gl/window.hpp>
#include <albedo/exception.hpp>

using abd::gl::window;
using abd::gl::window_builder;


window::window(int resx, int resy, const std::string &title, const window_builder &builder) :
	m_window(nullptr, nullptr)
{
	// Set all hints
	glfwDefaultWindowHints();
	for (const auto &h : builder.to_hints())
		window::set_global_hint(h);

	m_window = decltype(m_window){glfwCreateWindow(resx,
		resy,
		title.c_str(),
		builder.get_monitor(),
		builder.get_parent_window()), glfwDestroyWindow};

	if (!m_window)
		throw abd::exception("failed to create GLFW window");

	// Make context current and initialize context
	make_current();
	if (glewInit() != GLEW_OK)
		throw abd::exception("glewInit() failed during window creation");
}

void window::make_current() const
{
	glfwMakeContextCurrent(m_window.get());
}

bool window::should_close() const
{
	return glfwWindowShouldClose(m_window.get());
}

void window::swap_buffers()
{
	return glfwSwapBuffers(m_window.get());
}

std::vector<window::hint> window_builder::to_hints() const
{
	return std::vector<window::hint>{
		{GLFW_OPENGL_FORWARD_COMPAT, m_forward_compat},
		{GLFW_OPENGL_DEBUG_CONTEXT, m_debug},
		{GLFW_CONTEXT_VERSION_MAJOR, m_major},
		{GLFW_CONTEXT_VERSION_MINOR, m_minor},
		{GLFW_OPENGL_PROFILE, m_profile},
		{GLFW_SAMPLES, m_samples},
	};
}