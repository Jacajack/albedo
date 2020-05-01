#pragma once

#include <albedo/resource_manager.hpp>
#include <albedo/mesh.hpp>
#include <albedo/simple_loaders.hpp>
#include <memory>

namespace abd {

/**
	Default resource engine type that handles all resource types
	(all classes that are derived from abd::resource).
*/
using default_resource_engine = abd::resource_engine<mesh>;

/**
	Below are definitions of default resource loaders
*/

template <>
typename abd::resource<abd::mesh>::default_loader_type abd::resource<abd::mesh>::default_loader = 
[](const boost::filesystem::path &path)->std::unique_ptr<mesh>
{
	return std::make_unique<abd::mesh>(abd::assimp_simple_load_mesh(path.string()));
};

}