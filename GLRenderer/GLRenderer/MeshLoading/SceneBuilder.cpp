#include <GLRendererStdafx.h>

#include <GLRenderer/MeshLoading/SceneBuilder.h>

#include <GLRenderer/MeshLoading/Scene.h>
#include <GLRenderer/MeshLoading/SceneLoader.h>

#include <GLRenderer/Mesh/StaticMeshResource.h>

#include <GLRenderer/Textures/Texture.h>

#include <Physics/Primitives/AABB.h>

// #include "render/Nodes/MeshNode.h"
// #include "render/Nodes/RenderNode.h"
// #include "render/Nodes/Scene.h"
// #include "render/Nodes/Terrain.h"

#include <GLRenderer/Textures/TextureLoader.h>

#include <glm/gtc/matrix_transform.hpp>

//#include "Debug.h"

#include <pugixml.hpp>

namespace GLEngine {
namespace GLRenderer {
namespace Mesh {

//=================================================================================
C_SceneBuilder::C_SceneBuilder()
{
	m_nullTexture = std::make_shared<Textures::C_Texture>();
	m_nullTexture->StartGroupOp();
	m_nullTexture->SetFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
	m_nullTexture->SetWrap(GL_REPEAT, GL_REPEAT);
	GLubyte data[] = { 0, 0, 0, 255 };
	glTexImage2D(m_nullTexture->GetTarget(), 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	m_nullTexture->SetDimensions({ 1,1 });
	m_nullTexture->EndGroupOp();
}

//=================================================================================
/**
 * @method:		LoadScene
 * @fullName:	render::C_SceneBuilder::LoadScene
 * @access:		public
 * @return:		std::shared_ptr<render::C_Scene>
 * @qualifier:
 * @param: 		const std::string & sceneDefinitionFile - path from working directory
 *				to xml containing scene definition
 * @brief
 */
 //=================================================================================
	// used to be rendernode
/*
std::shared_ptr<render::C_Scene> C_SceneBuilder::LoadScene(const std::string& sceneDefinitionFile)
{
	m_sceneFolder = GetFolderpath(sceneDefinitionFile);

	pugi::xml_parse_result result;
	pugi::xml_document document;

	result = document.load_file(sceneDefinitionFile.c_str());
	if (!(result.status == pugi::status_ok)) {
		throw std::exception("Scene description file cannot be loaded!");
	}
	auto scene = std::make_shared<render::C_Scene>();

	for (auto& child : document.child("scene").children()) {
		if (!strcmp(child.name(), "terrain")) {
			scene->emplace_back(LoadTerrain(child));
		}
		else if (!strcmp(child.name(), "model")) {
			scene->emplace_back(LoadModel(child));
		}
	}

	Physics::Primitives::S_AABB bbox;

	for (const auto & node : *scene) {
		bbox.Add(node->GetActualAABB());
	}
	scene->SetBBox(bbox);

	return scene;
}

//=================================================================================
std::shared_ptr<render::C_Terrain> C_SceneBuilder::LoadTerrain(const pugi::xml_node& node)
{
	// assert(strcmp(node.name(), "terrain") == 0);
	// auto fileAtt = node.attribute("file");
	// 
	// TextureLoader tl;
	// 
	// Texture t;
	// bool retval = tl.loadTexture((m_sceneFolder + "/" + node.attribute("file").as_string()).c_str(), t);
	// 
	// 
	// if (!retval)
	// {
	// 	throw std::exception("terrain height map does not exists");
	// }
	// 
	// return std::make_shared<render::C_Terrain>(node.attribute("tile-size").as_float(1.0f), t);

	return nullptr;
}

//=================================================================================
std::shared_ptr<C_Scene> C_SceneBuilder::LoadModel(const pugi::xml_node& node)
{
	auto sl = std::make_unique<SceneLoader>();

	std::shared_ptr<Scene> scene = std::make_shared<Scene>();

	glm::mat4 modelMatrix = glm::mat4(1.0f);
	auto positionNode = node.child("position");
	//if (positionNode) {
	//	modelMatrix = glm::translate(modelMatrix, ReadPositionNode(positionNode));
	//}

	auto path = (m_sceneFolder + "/" + GetFolderpath(node.attribute("file").as_string()));

	if (!sl->addModelFromFileToScene(path.c_str(), GetFilePart(node.attribute("file").as_string()).c_str(), scene, modelMatrix))
	{
		std::cerr << "Unable to load model " << std::endl;
		return nullptr;
	}

	m_scene = scene;

	glm::vec3 position(0.f);

	if (positionNode) {
		position = ReadPositionNode(positionNode);
	}
	std::shared_ptr<C_Scene> ret = std::make_shared<C_Scene>(position);
	for (const auto&texture : scene->textures) {
		m_textures.push_back(LoadTexture(texture));
	}
	for (const auto &mesh : scene->meshes) {
		const auto& meshNode = LoadMesh(mesh);

		std::string query(R"(object[@name=")" + mesh.m_name + R"("])");
		auto objectNode = node.select_node(query.c_str()).node();
		if (objectNode) {
			auto isShadowcasterAttribute = objectNode.attribute("isShadowCaster");
			if (isShadowcasterAttribute) {
				meshNode->SetShadowCaster(isShadowcasterAttribute.as_bool());
			}
		}
		try
		{
			ret->push_back(std::move(meshNode));
		}
		catch (const std::exception& e)
		{
			std::cout << e.what() << std::endl;
		}
	}



	Physics::Primitives::S_AABB bbox;

	for (const auto & node : *ret) {
		bbox.Add(node->GetActualAABB());
	}
	ret->SetBBox(bbox);

	return ret;
}

//=================================================================================
std::shared_ptr<I_RenderNode> C_SceneBuilder::LoadMesh(const Mesh& mesh)
{
	auto node = std::make_shared<C_MeshNode>();
	node->LoadMesh(mesh);
	auto material = m_scene->materials[mesh.materialIndex];
	if (material.textureIndex != -1) {
		node->SetTexture(m_textures[material.textureIndex]);
	}
	node->SetColor(material.diffuse);

	node->SetName(mesh.m_name);

	return node;
}
*/

//=================================================================================
std::shared_ptr<Textures::C_Texture> C_SceneBuilder::LoadTexture(const Texture & texture) const
{
	auto tex = std::make_shared<Textures::C_Texture>(texture.m_name);
	tex->StartGroupOp();
	glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_RGB,
		(GLsizei)texture.width,
		(GLsizei)texture.height,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		texture.data.get());
	tex->SetDimensions({ texture.width, texture.height });
//	ErrorCheck();
	tex->SetWrap(GL_REPEAT, GL_REPEAT);
	tex->SetFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
//	ErrorCheck();

	tex->EndGroupOp();

	return tex;
}

//=================================================================================
glm::vec3 C_SceneBuilder::ReadPositionNode(const pugi::xml_node& node) const noexcept
{
	assert(!strcmp(node.name(), "position"));
	return glm::vec3(node.attribute("x").as_double(0.0f), node.attribute("y").as_double(0.0f), node.attribute("z").as_double(0.0f));
}

//=================================================================================
std::string C_SceneBuilder::GetFolderpath(const std::string& filePath) const
{
	std::size_t found = filePath.find_last_of("/\\");

	return filePath.substr(0, found);
}

//=================================================================================
std::string C_SceneBuilder::GetFilePart(const std::string& filePath) const
{
	std::size_t found = filePath.find_last_of("/\\");

	return filePath.substr(found + 1, std::string::npos);
}

}}}