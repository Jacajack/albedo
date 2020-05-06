#include <albedo/gl/window.hpp>
#include <albedo/exception.hpp>

using abd::gl::window;
using abd::gl::window_builder;


/**
	Passes keyboard event to the window's keyboard handler
*/
void window::keyboard_callback(GLFWwindow *win, int key, int scancode, int action, int mods)
{
	auto *cb = reinterpret_cast<control_block*>(glfwGetWindowUserPointer(win));
	if (cb) cb->keyboard.callback(key, scancode, action, mods);
}

/**
	Passes mouse button event to the window's mouse handler
*/
void window::mouse_button_callback(GLFWwindow *win, int button, int action, int mods)
{
	auto *cb = reinterpret_cast<control_block*>(glfwGetWindowUserPointer(win));
	if (cb) cb->mouse.button_callback(button, action, mods);
}

/**
	Passes mouse movement event to the window's mouse handler
*/
void window::cursor_position_callback(GLFWwindow *win, double x, double y)
{
	auto *cb = reinterpret_cast<control_block*>(glfwGetWindowUserPointer(win));
	if (cb) cb->mouse.position_callback(x, y);
}


window::window(int resx, int resy, const std::string &title, const window_builder &builder) :
	m_window(nullptr, nullptr),
	m_control_block(std::make_unique<control_block>())
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

	// Set GLFW user pointer and callbacks
	glfwSetWindowUserPointer(m_window.get(), m_control_block.get());
	glfwSetKeyCallback(m_window.get(), window::keyboard_callback);
	glfwSetCursorPosCallback(m_window.get(), window::cursor_position_callback);
	glfwSetMouseButtonCallback(m_window.get(), window::mouse_button_callback);

	// Make context current and initialize context
	make_current();
	if (glewInit() != GLEW_OK)
		throw abd::exception("glewInit() failed during window creation");
}

window::keyboard_handler &window::get_keyboard_handler()
{
	return m_control_block->keyboard;
}

window::mouse_handler &window::get_mouse_handler()
{
	return m_control_block->mouse;
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


void window::keyboard_handler::callback(int key, int scancode, int action, int mods)
{
	m_keyboard_status[scancode] = {action, mods, action != GLFW_RELEASE};
}

void window::mouse_handler::button_callback(int button, int action, int mods)
{
	if (m_button_func)
		m_button_func(button, action, mods);
}

void window::mouse_handler::position_callback(double x, double y)
{
	m_x = x;
	m_y = y;
}