#include <GLRendererStdafx.h>

#include <GLRenderer/Textures/TextureLoader.h>
#include <GLRenderer/MeshLoading/SceneLoader.h>
#include <GLRenderer/MeshLoading/ModelLoader.h>

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
bool SceneLoader::addModelFromDAEFileToScene(const char* filepath, const char* filename, std::shared_ptr<C_StaticMeshResource>& oMesh, std::string& textureName, const glm::mat4& transform /*= glm::mat4(1)*/)
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
							vertices.emplace_back(x, y, z, 1.f);
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

	return true;
}

}