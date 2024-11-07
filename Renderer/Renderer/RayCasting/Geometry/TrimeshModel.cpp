#include <RendererStdafx.h>

#include <Renderer/RayCasting/Geometry/BVH.h>
#include <Renderer/RayCasting/Geometry/TrimeshModel.h>

#include <Core/Resources/ResourceManager.h>

#include <Utils/HighResolutionTimer.h>
#include <Utils/Serialization/XMLDeserialize.h>
#include <Utils/Serialization/XMLSerialize.h>

#include <rttr/registration>

// clang-format off
RTTR_REGISTRATION
{
	using namespace GLEngine::Renderer;
	using namespace GLEngine::Core;
	rttr::registration::class_<C_TrimeshModel>((C_TrimeshModel::GetResrourceTypeName() + "Handle").c_str())
		.constructor<>()(rttr::policy::ctor::as_std_shared_ptr)
		.method("AfterDeserialize", &C_TrimeshModel::AfterDeserialize)
		.property("Trimeshes", &C_TrimeshModel::m_Trimeshes)(rttr::policy::prop::bind_as_ptr)
		.property("BVHs", &C_TrimeshModel::m_BVHs)(rttr::policy::prop::bind_as_ptr);

		rttr::type::register_wrapper_converter_for_base_classes<std::shared_ptr<C_TrimeshModel>>();
		rttr::type::register_converter_func([](std::shared_ptr<C_TrimeshModel> ptr, bool& ok) -> std::shared_ptr<Resource> {                                          
			ok = true;                                                                                                                                                
			return std::static_pointer_cast<Resource>(ptr);                                                                                                           
		});
}
// clang-format on

namespace GLEngine::Renderer {

//=================================================================================
bool C_TrimeshModel::Reload()
{
	return false;
}

//=================================================================================
bool C_TrimeshModel::Load(const std::filesystem::path& filepath)
{
	m_Filepath = filepath;
	pugi::xml_document doc;

	pugi::xml_parse_result result;
	result = doc.load_file(m_Filepath.c_str());
	if (!result.status == pugi::status_ok)
	{
		CORE_LOG(E_Level::Error, E_Context::Core, "Can't open config file for trimesh name: {}", m_Filepath);
		return false;
	}
	Utils::C_XMLDeserializer d(Core::C_ResourceManager::Instance());
	auto					 newTrimesh = d.Deserialize<std::shared_ptr<C_TrimeshModel>>(doc);
	if (newTrimesh.has_value() == false)
	{
		CORE_LOG(E_Level::Error, E_Context::Core, "XML {} is not valid trimesh.", m_Filepath);
		return false;
	}
	std::swap(newTrimesh->get()->m_Trimeshes, m_Trimeshes);
	std::swap(newTrimesh->get()->m_BVHs, m_BVHs);
	return true;
}

//=================================================================================
bool C_TrimeshModel::Build(Core::ResourceHandle<MeshResource> hanlde)
{
	GLE_ASSERT(hanlde.IsReady(), "Building trimesh before MeshResource loaded.");
	if (!hanlde.IsReady())
		return false;
	for (auto& mesh : hanlde.GetResource().GetScene().meshes)
	{
		auto& trimesh = m_Trimeshes.emplace_back();
		// need to do something with material
		trimesh.AddMesh(mesh);
		trimesh.SetTransformation(mesh.modelMatrix);

		// bvh if needed
		if (trimesh.GetNumTriangles() > 10)
		{
			::Utils::HighResolutionTimer BVHBuildTime;
			auto&						 bvh = m_BVHs.emplace_back(new BVH(trimesh));
			bvh->Build();

			CORE_LOG(E_Level::Info, E_Context::Render, "BVH trimesh {} build time {}ms", mesh.m_name, BVHBuildTime.getElapsedTimeFromLastQueryMilliseconds());
			trimesh.SetBVH(bvh);
		}
		else
		{
			m_BVHs.push_back(nullptr);
		}
		break;
	}
	m_Dirty = true;
	return true;
}

//=================================================================================
bool C_TrimeshModel::SupportSaving() const
{
	return true;
}

//=================================================================================
bool C_TrimeshModel::SaveInternal() const
{
	Utils::C_XMLSerializer s;
	const auto			   str = s.Serialize(*this);
	return str.save_file(m_Filepath.c_str());
}

//=================================================================================
void C_TrimeshModel::AfterDeserialize()
{
	CORE_LOG(E_Level::Error, E_Context::Core, "Invalid resource {}.", m_Filepath);
	for (int i = 0; i < m_BVHs.size() && i < m_Trimeshes.size(); ++i)
	{
		m_BVHs[i]->m_Storage = &m_Trimeshes[i].m_Vertices;
		m_Trimeshes[i].m_BVH = m_BVHs[i];
	}
}

//=================================================================================
std::shared_ptr<GLEngine::Core::Resource> TrimeshModelTrimesh::CreateResource() const
{
	return std::make_shared<C_TrimeshModel>();
}

//=================================================================================
std::vector<std::string> TrimeshModelTrimesh::GetSupportedExtensions() const
{
	return {".tri"};
}

} // namespace GLEngine::Renderer
