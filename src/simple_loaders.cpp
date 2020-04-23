#include <albedo/simple_loaders.hpp>
#include <albedo/mesh.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>



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