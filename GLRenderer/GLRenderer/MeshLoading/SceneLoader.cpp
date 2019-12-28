#include <GLRendererStdafx.h>

#include <GLRenderer/Textures/TextureLoader.h>
#include <GLRenderer/MeshLoading/SceneLoader.h>
#include <GLRenderer/MeshLoading/ModelLoader.h>

#include <Renderer/Animation/Skeleton.h>
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
bool SceneLoader::addModelFromDAEFileToScene(const char* filepath, const char* filename, std::shared_ptr<C_StaticMeshResource>& oMesh, std::string& textureName, Renderer::Animation::C_Skeleton& skeleton, const glm::mat4& transform /*= glm::mat4(1)*/)
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
					std::string_view positionsString = xmlSource.child("float_array").child_value();
					std::stringstream ss;
					ss << positionsString;
					std::string_view id = xmlSource.attribute("id").as_string();

					auto floatCount = xmlSource.child("float_array").attribute("count").as_uint();

					if (id.find("positions") != id.npos)
					{
						vertices.reserve(floatCount/3);
						float x, y, z;
						while (ss >> x >> y >> z)
						{
							vertices.emplace_back(noramlizingMatrix * glm::vec4(x, y, z, 1.f));
						}
					}
					else if (id.find("normals") != id.npos)
					{
						normals.reserve(floatCount/3);
						float x, y, z;
						while (ss >> x >> y >> z)
						{
							normals.emplace_back(x, y, z);
						}
					}
					else if (id.find("map-0") != id.npos)
					{
						texCoords.reserve(floatCount/2);
						float s, t;
						while (ss >> s >> t)
						{
							texCoords.emplace_back(s ,t);
						}
					}
				}

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
			}

			oMesh = std::make_shared<C_StaticMeshResource>(mesh);
		}
	}

	if (auto controllerLibrary = colladaNode.child("library_controllers"))
	{
		if (auto skinXML = controllerLibrary.child("controller").child("skin"))
		{
			std::vector<Renderer::Animation::S_Joint> joints;
			std::vector<std::string> jointNames;
			LoadJoints(jointNames, skinXML);
			LoadJointsInvMatrices(jointNames, joints, skinXML, noramlizingMatrix);

			skeleton.m_Root = std::make_unique<Renderer::Animation::S_Joint>(joints[0]);

			// wrong!!
			skeleton.m_Root->m_Children.emplace_back(joints[1]);
			skeleton.m_Root->m_Children[0].m_Children.emplace_back(joints[2]);
			//skeleton.m_Root->m_Children.emplace_back(joints[10]);
			//skeleton.m_Root->m_Children.emplace_back(joints[13]);
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
void SceneLoader::LoadJointsInvMatrices(const std::vector<std::string>& jointNames, std::vector<Renderer::Animation::S_Joint>& joints, const pugi::xml_node& skinXML, const glm::mat4& normalizinMatrix) const
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
			joints.reserve(jointNames.size());

			const std::string_view matrices = floatArray.child_value();
			std::stringstream ss;
			ss << matrices;
			int i  = 0;
			for (const auto& name : jointNames)
			{
				float i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14, i15;
				ss >> i0 >> i1 >> i2 >> i3 >> i4 >> i5 >> i6 >> i7 >> i8 >> i9 >> i10 >> i11 >> i12 >> i13 >> i14 >> i15;
				joints.emplace_back(i, name, glm::transpose(normalizinMatrix*glm::mat4(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14, i15)));
				++i;
			}
		}
	}
}

}