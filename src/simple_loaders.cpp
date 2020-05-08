#include <albedo/simple_loaders.hpp>
#include <albedo/mesh.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <fstream>
#include <string>
#include <algorithm>
#include <iostream>
#include <boost/range/iterator_range.hpp>

abd::mesh_data abd::assimp_simple_load_mesh(const boost::filesystem::path &path)
{
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(path.string(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		throw abd::exception("assimp import error");
	}
	

	// The mesh data, we're going to return
	abd::mesh_data mesh_data;
	
	// Process materials
	//! \todo replace the fucking resource_manager with something useful and fix this mess
	std::vector<std::shared_ptr<material>> mats;
	for (unsigned int i = 0; i < scene->mNumMaterials; i++)
	{
		const auto &mat = *scene->mMaterials[i];
		material_data data;

		aiString str;
		mat.Get(AI_MATKEY_NAME, str);
		std::cout << str.C_Str() << std::endl;


		// Diffuse term
		aiColor3D diffuse_color;
		mat.Get(AI_MATKEY_COLOR_DIFFUSE, diffuse_color);
		data.diffuse.r = diffuse_color.r;
		data.diffuse.g = diffuse_color.g;
		data.diffuse.b = diffuse_color.b;

		// std::cout << "bright: " << data.diffuse.length() << std::endl;

		// Shininess (specular exponent)
		float shininess;
		mat.Get(AI_MATKEY_SHININESS, shininess);
		
		// Shininess 900 --> roughness 0
		// Shininess 0   --> roughness 1
		// std::cout << shininess << std::endl;
		data.roughness = 1 - std::pow(shininess / 900.f, 2.f);
		// std::cout << data.roughness << std::endl;

		// Specular amount
		mat.Get(AI_MATKEY_SHININESS_STRENGTH, data.specular);
		// data.specular = 1;

		mats.emplace_back(std::make_shared<material>(data));
	}

	// Appends aiMesh to the mesh we're working on
	auto process_mesh = [&](aiMesh *mesh)
	{
		// Register new base index
		mesh_data.base_indices.push_back(mesh_data.indices.size());

		// Process vertices
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			// Vertex position
			mesh_data.positions.emplace_back(
				mesh->mVertices[i].x,
				mesh->mVertices[i].y,
				mesh->mVertices[i].z);

			// Vertex normal
			mesh_data.normals.emplace_back(
				mesh->mNormals[i].x,
				mesh->mNormals[i].y,
				mesh->mNormals[i].z);

			// UVs - only the first texture
			if (mesh->mTextureCoords[0])
			{
				mesh_data.uvs.emplace_back(
					mesh->mTextureCoords[0][i].x,
					mesh->mTextureCoords[0][i].y);
			}
		}

		// Check if there is equal number of vertex positions and normals (and uvs)
		if (mesh_data.positions.size() != mesh_data.normals.size()
			|| (mesh_data.uvs.size() > 0 && mesh_data.uvs.size() != mesh_data.positions.size()))
		{
			throw abd::exception("attempted to load a mesh with missing normals or UVs");
		}

		// Indices
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			const auto &f = mesh->mFaces[i];
			for (unsigned int j = 0; j < f.mNumIndices; j++)
				mesh_data.indices.push_back(f.mIndices[j]); 
		}

		// Assuming all faces are triangles
		mesh_data.draw_sizes.push_back(mesh->mNumFaces * 3);

		// Material
		mesh_data.materials.push_back(mats.at(mesh->mMaterialIndex));
	};

	// Adds all node's meshes to the compound mesh
	const auto process_node = [&](const aiScene *scene, aiNode *node)
	{
		auto process_node_impl = [&](const aiScene *scene, aiNode *node, auto &ref)->void
		{
			// Process all meshes
			for (unsigned int i = 0; i < node->mNumMeshes; i++)
				process_mesh(scene->mMeshes[node->mMeshes[i]]);

			// Process children
			for (unsigned int i = 0; i < node->mNumChildren; i++)
				ref(scene, node->mChildren[i], ref);
		};

		process_node_impl(scene, node, process_node_impl);
	};

	process_node(scene, scene->mRootNode);
	return mesh_data;
}

abd::gl::shader abd::simple_load_shader(GLenum type, const boost::filesystem::path &path)
{
	std::ifstream f{path.string()};
	if (!f) throw abd::exception("could not open shader source file");

	std::string src;
	std::copy(std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>(), std::back_insert_iterator(src));

	return abd::gl::shader(type, src);
}

abd::gl::program abd::simple_load_shader_dir(const boost::filesystem::path &dir)
{
	using namespace boost::filesystem;

	if (!exists(dir))
		throw abd::exception("shader dir does not exist");

	if (!is_directory(dir))
		throw abd::exception("shader program must be provided in a directory on its own");

	// Loaded shaders
	std::vector<abd::gl::shader> shaders;

	// Shader extensions mapped to shader types
	static const std::unordered_map<std::string, GLenum> shader_types = {
		{".vs", GL_VERTEX_SHADER},
		{".fs", GL_FRAGMENT_SHADER},
		{".tcs", GL_TESS_CONTROL_SHADER},
		{".tes", GL_TESS_EVALUATION_SHADER},
		{".cs", GL_COMPUTE_SHADER},
		{".gs", GL_GEOMETRY_SHADER},
	};

	// Iterate over directory contents
	for (auto &entry : boost::make_iterator_range(directory_iterator(dir), {}))
	{
		auto path = entry.path();

		// If the path end with .glsl, it's likely a hit
		if (is_regular_file(path) && path.extension() == ".glsl")
		{
			auto shader_type_str = path.stem().extension().string();

			try
			{
				GLenum type = shader_types.at(shader_type_str);
				shaders.emplace_back(simple_load_shader(type, path));
			}
			catch (const std::out_of_range &ex)
			{
				// Ignore
			}
		}
	}

	// Did not find any shaders
	if (shaders.empty())
		throw abd::exception("could not find any shaders in the provided directory");

	return abd::gl::program(shaders);
}