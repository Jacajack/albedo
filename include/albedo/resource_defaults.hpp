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

/*
	Below are specializations of default loaders for differend resource types
*/

template <>
typename abd::resource<abd::mesh_data>::default_loader_type abd::resource<abd::mesh_data>::default_loader = 
[](const boost::filesystem::path &path)->std::unique_ptr<mesh_data>
{
	return std::make_unique<abd::mesh_data>(abd::assimp_simple_load_mesh(path.string()));
};

}