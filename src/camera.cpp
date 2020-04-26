#include <albedo/camera.hpp>

using abd::perspective;
using abd::camera;

perspective::perspective(float fov, float aspect, float near, float far) :
	m_fov(fov),
	m_aspect_ratio(aspect),
	m_near(near),
	m_far(far)
{
	update_matrix();	
}

const glm::mat4 &perspective::get_matrix() const 
{
	return m_matrix;
}

perspective::operator glm::mat4() const 
{
	return m_matrix;
}

void perspective::set_fov(float fov)
{
	m_fov = fov;
	update_matrix();
}

void perspective::set_fov(float focal_length, float frame_width)
{
	m_fov =  2 * std::atan2(frame_width, focal_length * 2);
	update_matrix();
}

float perspective::get_fov() const
{
	return m_fov;
}

void perspective::set_aspect_ratio(float aspect)
{
	m_aspect_ratio = aspect;
	update_matrix();
}

float perspective::get_aspect_ratio() const
{
	return m_aspect_ratio;
}

void perspective::set_near_plane(float near)
{
	m_near = near;
	update_matrix();
}

float perspective::get_near_plane() const
{
	return m_near;
}

void perspective::set_far_plane(float far)
{
	m_far = far;
	update_matrix();
}

float perspective::get_far_plane() const
{
	return m_far;
}

/**
	Internal matrix updater called after each set operation
*/
void perspective::update_matrix()
{
	m_matrix = glm::perspective(m_fov, m_aspect_ratio, m_near, m_far);
}



camera::camera(const glm::vec3 &pos, const glm::vec3 &target, const glm::vec3 &up, const glm::mat4 &proj) :
	m_mat_proj(proj),
	m_pos(pos),
	m_up(up)
{
	look_at(target);
}

void camera::set_projection_matrix(const glm::mat4 &mat)
{
	m_mat_proj = mat;
	update_matrix();
}

glm::mat4 camera::get_projection_matrix() const
{
	return m_mat_proj;
}

void camera::set_direction(const glm::vec3 &dir)
{
	m_direction = dir;
	update_view_matrix();
}

void camera::look_at(const glm::vec3 &where)
{
	m_direction = glm::normalize(where - m_pos);
	update_view_matrix();
}

const glm::vec3 &camera::get_direction() const
{
	return m_direction;
}

const glm::mat4 &camera::get_matrix() const
{
	return m_matrix;
}

camera::operator glm::mat4() const
{
	return m_matrix;
}

void camera::update_view_matrix()
{
	m_mat_view = glm::lookAt(m_pos, m_pos + m_direction, m_up);
	update_matrix();
}

void camera::update_matrix()
{
	m_matrix = m_mat_proj * m_mat_view;
}
