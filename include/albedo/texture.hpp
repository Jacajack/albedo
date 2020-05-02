#pragma once

#include <albedo/gl/texture.hpp>

namespace abd {

/**
	This is a normal 2D texture
	\warning Do not confuse with abd::gl::texture_2d.
*/
using texture_2d = abd::gl::texture<abd::gl::texture_target::TEXTURE_2D>;

/**
	By default, textures are two-dimensional
*/
using texture = texture_2d;

}