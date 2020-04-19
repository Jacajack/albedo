#include <albedo/gl/window.hpp>
#include <albedo/exception.hpp>

using abd::gl::window;

window::window(int resx, int resy, const std::string &title, GLFWmonitor *monitor, GLFWwindow *share, std::initializer_list<hint> hints)
	: m_window(nullptr, nullptr)
{
	// Set all hints
	glfwDefaultWindowHints();
	for (const auto &h : hints)
		window::set_global_hint(h);

	m_window = decltype(m_window)(glfwCreateWindow(resx, resy, title.c_str(), monitor, share), glfwDestroyWindow);

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