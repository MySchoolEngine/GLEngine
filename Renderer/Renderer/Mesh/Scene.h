#pragma once

#include <Renderer/Animation/Skeleton.h>
#include <Renderer/Colours.h>

#include <Physics/Primitives/AABB.h>

namespace GLEngine::Renderer::MeshData {

//=================================================================================
struct Material {
	Colours::T_Colour ambient;
	glm::vec4		  diffuse;
	Colours::T_Colour specular;
	float			  shininess;

	// Index to a texture array
	// If negative - material has no texture
	int textureIndex	   = -1;
	int normalTextureIndex = -1;

	std::string m_Name;
};

//=================================================================================
struct SkeletonData {
	struct BoneInfo {
		JointID	  ID;
		glm::mat4 modelSpace;
		glm::mat4 localSpace;

		JointID				 parent;
		std::vector<JointID> children;
	};

	using T_BoneIndex							 = unsigned int;
	inline constexpr static T_BoneIndex BadIndex = static_cast<T_BoneIndex>(-1);
	static_assert(std::is_unsigned<T_BoneIndex>::value);
	std::vector<glm::ivec3>		   jointIndices;
	std::vector<glm::vec3>		   weights;
	std::vector<BoneInfo>		   bones;
	std::map<JointID, T_BoneIndex> boneMapping;

	void AddBoneData(int vertexID, int boneID, float weight)
	{
		for (unsigned int i = 0; i < 3; ++i)
		{
			if (weights[vertexID][i] == 0.f)
			{
				weights[vertexID][i]	  = weight;
				jointIndices[vertexID][i] = boneID;

				return;
			}
		}
	}

	[[nodiscard]] T_BoneIndex GetBoneIndex(JointID jointID) const
	{
		const auto boneInfo = boneMapping.find(jointID);
		if (boneInfo == boneMapping.end())
			return BadIndex;
		else
			return boneInfo->second;
	}
};

//=================================================================================
struct Mesh {
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texcoords;
	std::vector<glm::vec3> tangent;
	std::vector<glm::vec3> bitangent;

	Physics::Primitives::S_AABB bbox;
	glm::mat4					modelMatrix;
	unsigned int				materialIndex;
	std::string					m_name; // DR

	SkeletonData skeleton;
};

//=================================================================================
struct Light {
	std::string		  m_name;
	Colours::T_Colour m_Color;
};

// Texture
// Always R8G8B8A8 format
//=================================================================================
struct Texture {
	Texture()
		: width(0)
		, height(0)
		, data(nullptr)
	{
	}

	unsigned int width;
	unsigned int height;

	std::string m_name;

	std::shared_ptr<unsigned char> data;
};

//=================================================================================
struct Scene {
	// This is the data that interests You
	std::vector<Mesh>  meshes;
	std::vector<Light> lights;

	std::vector<Material>			   materials;
	std::vector<std::filesystem::path> textures;

	Physics::Primitives::S_AABB bbox;
};
} // namespace GLEngine::Renderer::MeshData