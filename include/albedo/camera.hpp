#pragma once

#include <albedo/gl/gl.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace abd {

/**
	A projection abstract base class
*/
class projection
{
public:
	virtual const glm::mat4 &get_matrix() const = 0;
	virtual operator glm::mat4() const = 0;
	virtual ~projection() = default;
};

/**
	A perspective projection
*/
class perspective : public projection
{
public:
	perspective(float fov, float aspect, float near, float far);
	virtual ~perspective() = default;

	virtual const glm::mat4 &get_matrix() const override;
	virtual operator glm::mat4() const override;

	void set_fov(float fov);
	void set_fov(float focal_length, float frame_width);
	float get_fov() const;

	void set_aspect_ratio(float aspect);
	float get_aspect_ratio() const;

	void set_near_plane(float near);
	float get_near_plane() const;

	void set_far_plane(float far);
	float get_far_plane() const;

private:
	void update_matrix();

	glm::mat4 m_matrix;
	float m_fov;
	float m_aspect_ratio;
	float m_near;
	float m_far;
};

/**
	A simple camera
*/
class camera
{
public:
	camera(const glm::vec3 &pos, const glm::vec3 &target, const glm::vec3 &up = {0, 1, 0}, const glm::mat4 &proj = perspective(glm::radians(60.f), 16.f / 9.f, 0.001f, 1000.f));

	// Perspective settings
	void set_projection_matrix(const glm::mat4 &mat);
	glm::mat4 get_projection_matrix() const;

	// For moving the camera
	void set_position(const glm::vec3 &pos);
	const glm::vec3 &get_position() const;
	
	// For pointing the camera
	void set_direction(const glm::vec3 &dir);
	void look_at(const glm::vec3 &where);
	const glm::vec3 &get_direction() const;

	// For retrieving view-projection matrix
	const glm::mat4 &get_matrix() const;
	operator glm::mat4() const;

private:
	void update_view_matrix();
	void update_matrix();

	glm::mat4 m_mat_view;
	glm::mat4 m_mat_proj;
	glm::mat4 m_matrix;

	glm::vec3 m_pos;
	glm::vec3 m_direction;
	glm::vec3 m_up;
};

}