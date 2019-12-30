#include <GLRendererStdafx.h>

#include <GLRenderer/MeshLoading/SceneLoader.h>

#include <GLRenderer/Textures/TextureLoader.h>
#include <GLRenderer/MeshLoading/ModelLoader.h>
#include <GLRenderer/MeshLoading/Collada/FloatArray.h>

#include <Renderer/Animation/Skeleton.h>
#include <Renderer/Animation/SkeletalAnimation.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

#include <pugixml.hpp>

namespace GLEngine::GLRenderer::Mesh {

//=================================================================================
bool SceneLoader::addModelFromFileToScene(const char* filepath, const char* filename, 
											std::shared_ptr<Scene> scene, const glm::mat4& transform)
{
	//Load geometry first
	ModelLoader ml;
	// dr
	ml.Reset();
	std::vector< std::string > texNames;

	bool retval = ml.addModelFromFileToScene((std::string(filepath) + std::string("/") + std::string(filename)).c_str(), scene, texNames, transform);

	if (!retval)
		return false;

	for (const auto& mesh : scene->meshes)
		scene->bbox.Add(mesh.bbox.getTransformedAABB(mesh.modelMatrix));

	Textures::TextureLoader tl;

	for (const auto & texName : texNames)
	{
		Texture t;
		retval = tl.loadTexture((std::string(filepath) + std::string("\\") + texName).c_str(), t);

		if (!retval)
			return false;

		scene->textures.push_back(t);
	}

	return true;
}

//=================================================================================
bool SceneLoader::addModelFromDAEFileToScene(
	const char* filepath, 
	const char* filename, 
	std::shared_ptr<C_StaticMeshResource>& oMesh, 
	std::string& textureName, 
	Renderer::Animation::C_Skeleton& skeleton, 
	Renderer::Animation::C_SkeletalAnimation& animation, 
	const glm::mat4& transform /*= glm::mat4(1)*/)
{
	std::string name;
	name.append(filepath);
	name.append("/");
	name.append(filename);
	CORE_LOG(E_Level::Info, E_Context::Core, "Loading dae file: {}", name);
	pugi::xml_document doc;

	pugi::xml_parse_result result;
	result = doc.load_file(name.c_str());
	if (!result.status == pugi::status_ok) {
		CORE_LOG(E_Level::Error, E_Context::Core, "Can't open dae file: {}", name);
		return false;
	}

	auto colladaNode = doc.child("COLLADA");
	if (!colladaNode)
	{
		CORE_LOG(E_Level::Error, E_Context::Core, "Invalid dae file: {}", name);
		return false;
	}

	glm::mat4 noramlizingMatrix(1.f);

	if (auto up_Axis = colladaNode.child("asset").child("up_axis"))
	{
		// default is Y_UP, which is also default for me
		std::string_view upAxe = up_Axis.child_value();
		if (upAxe == "Z_UP")
		{
			noramlizingMatrix = glm::rotate(-glm::half_pi<float>(), glm::vec3(1.f, .0f, .0f));
		}
		else if (upAxe == "Y_UP")
		{
			//do nothing
		}
		else
		{
			CORE_LOG(E_Level::Error, E_Context::Core, "unknown <up_axis> value: {}", upAxe);
		}
	}

	if (auto imageLibrary = colladaNode.child("library_images").child("image").child("init_from"))
	{
		textureName = imageLibrary.child_value();
	}

	if (auto geomLibrary = colladaNode.child("library_geometries"))
	{
		for (const auto geom : geomLibrary.children("geometry"))
		{
			std::vector<glm::vec4> vertices;
			std::vector<glm::vec3> normals;
			std::vector<glm::vec2> texCoords;
			std::string_view id = geom.attribute("name").as_string();
			Mesh mesh;
			mesh.m_name = geom.attribute("name").as_string();
			if (auto xmlMesh = geom.child("mesh"))
			{
				for (auto xmlSource : xmlMesh.children("source"))
				{
					S_FloatArray floats(xmlSource.child("float_array"));

					std::string_view id = xmlSource.attribute("id").as_string();

					if (id.find("positions") != id.npos)
					{
						vertices.reserve(floats.count<glm::vec3>());
						while (!floats.EndOfArray())
						{
							vertices.emplace_back(noramlizingMatrix * glm::vec4(floats.Get<glm::vec3>(), 1.f));
						}
					}
					else if (id.find("normals") != id.npos)
					{
						normals.reserve(floats.count<glm::vec3>());
						while (!floats.EndOfArray())
						{
							normals.emplace_back(floats.Get<glm::vec3>());
						}
					}
					else if (id.find("map-0") != id.npos)
					{
						texCoords.reserve(floats.count<glm::vec2>());
						while (!floats.EndOfArray())
						{
							texCoords.emplace_back(floats.Get<glm::vec2>());
						}
					}
				}

				// here it is possible to non-triangle polygons to arise
				if (auto polylist = xmlMesh.child("polylist"))
				{
					if (auto indiceList = polylist.child("p"))
					{
						std::string_view indiceListString = indiceList.child_value();
						std::stringstream indicesStream;
						indicesStream << indiceListString;
						int v, n, t, c;
						while (indicesStream >> v >> n >> t >> c)
						{
							mesh.vertices.push_back(vertices[v]);
							mesh.normals.emplace_back(normals[n]);
							mesh.texcoords.emplace_back(texCoords[t]);
						}
					}
				}
				else if (auto triangles = xmlMesh.child("triangles"))
				{
					CORE_LOG(E_Level::Error, E_Context::Render, "<Trianlges> not supported yet");
				}
			}

			oMesh = std::make_shared<C_StaticMeshResource>(mesh);
		}
	}

	std::map<std::string, Renderer::Animation::S_Joint> joints;
	if (auto controllerLibrary = colladaNode.child("library_controllers"))
	{
		if (auto skinXML = controllerLibrary.child("controller").child("skin"))
		{
			std::vector<std::string> jointNames;
			LoadJoints(jointNames, skinXML);
			LoadJointsInvMatrices(jointNames, joints, skinXML, noramlizingMatrix);
		}
	}

	if (auto visualScene = colladaNode.child("library_visual_scenes").child("visual_scene"))
	{
		for (auto xmlNode : visualScene.children("node"))
		{
			// we should take id from library_controllers
			std::string_view id = xmlNode.attribute("id").as_string();
			if (id == "Armature")
			{
				// ignore rotation/scale matrices
				auto modelMatrix = ParseTranslation(xmlNode);
				if (std::distance(xmlNode.children("node").begin(), xmlNode.children("node").end()) != 1)
				{
					CORE_LOG(E_Level::Warning, E_Context::Render, "Skeleton definition should contain only 1 root joint");
				}

				auto rootNode = *(xmlNode.children("node").begin());
				std::string_view boneName = rootNode.attribute("sid").as_string();
				const auto rootJoint = joints.find(boneName.data());
				if (rootJoint == joints.end())
				{
					CORE_LOG(E_Level::Error, E_Context::Render, "Bone '{}' referenced in file '{}' doesn't exiests.", boneName, name);
					return false;
				}
				skeleton.m_Root = std::make_unique<Renderer::Animation::S_Joint>(rootJoint->second);
				if (!ParseChildrenJoints(*skeleton.m_Root.get(), rootNode, joints))
				{
					CORE_LOG(E_Level::Error, E_Context::Render, "Errors in joint hierarchy definition in file: {}", name);
				}
			}
		}
	}

	return true;
}

//=================================================================================
void SceneLoader::LoadJoints(std::vector<std::string>& jointNames, const pugi::xml_node& skinXML) const
{
	for (auto xmlSource : skinXML.children("source"))
	{
		const std::string_view id = xmlSource.attribute("id").as_string();
		if (id.find("joints") == id.npos)
		{
			continue;
		}

		if (const auto nameArray = xmlSource.child("Name_array"))
		{
			const std::size_t numBones = nameArray.attribute("count").as_int();
			jointNames.reserve(numBones);
			const std::string_view names = nameArray.child_value();
			std::stringstream ss;
			ss << names;
			std::string name;
			while (ss >> name)
			{
				jointNames.emplace_back(name);
			}
		}
	}
}

//=================================================================================
void SceneLoader::LoadJointsInvMatrices(const std::vector<std::string>& jointNames, std::map<std::string, Renderer::Animation::S_Joint>& joints, const pugi::xml_node& skinXML, const glm::mat4& normalizinMatrix) const
{
	for (auto xmlSource : skinXML.children("source"))
	{
		const std::string_view id = xmlSource.attribute("id").as_string();
		if (id.find("bind_poses") == id.npos)
		{
			continue;
		}

		if (const auto floatArray = xmlSource.child("float_array"))
		{
			S_FloatArray floats(floatArray);
			int i  = 0;
			for (const auto& name : jointNames)
			{
				const auto bindMatrix = glm::transpose(normalizinMatrix * floats.Get<glm::mat4>());
				joints.insert({ name, Renderer::Animation::S_Joint(i, name, bindMatrix) });
				++i;
			}
		}
	}
}

//=================================================================================
glm::mat4 SceneLoader::ParseTranslation(const pugi::xml_node& node)
{
	float x, y, z;
	if (auto translation = node.child("translate"))
	{
		std::stringstream ss;
		ss << translation.child_value();
		ss >> x >> y >> z;
		return glm::translate(glm::mat4(1.f), { x,y,z });
	}
	return glm::mat4(1.f);
}

//=================================================================================
bool SceneLoader::ParseChildrenJoints(Renderer::Animation::S_Joint& parent, const pugi::xml_node& xmlParent, const std::map<std::string, Renderer::Animation::S_Joint>& joints)
{
	for (const auto& node : xmlParent.children("node"))
	{
		std::string_view boneName = node.attribute("sid").as_string();
		const auto joint = joints.find(boneName.data());
		if (joint == joints.end())
		{
			CORE_LOG(E_Level::Error, E_Context::Render, "Bone '{}' referenced doesn't exiests.", boneName);
			continue;
			//return false;
		}
		auto& includedJoint = parent.m_Children.emplace_back(joint->second);
		if (!ParseChildrenJoints(includedJoint, node, joints))
		{
			return false;
		}
	}
	return true;
}

}