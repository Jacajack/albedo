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
using default_resource_engine_type = abd::resource_engine<mesh>;

/**
	A singleton class representing the main resource_engine of the entire application
*/
class main_resource_engine
{
public:
	static default_resource_engine_type &get();

private:
	static std::unique_ptr<default_resource_engine_type> m_engine;
};

/**
	Below are definitions of default resource loaders
*/

template <>
inline typename abd::resource<abd::mesh>::default_loader_type abd::resource<abd::mesh>::default_loader = 
[](const boost::filesystem::path &path)->std::unique_ptr<mesh>
{
	return std::make_unique<abd::mesh>(abd::assimp_simple_load_mesh(path.string()));
};

}