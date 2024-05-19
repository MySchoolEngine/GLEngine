#include <RendererStdafx.h>

#include <Renderer/RayCasting/Geometry/TrimeshModel.h>
#include <Renderer/RayCasting/Geometry/BVH.h>

#include <Utils/HighResolutionTimer.h>

namespace GLEngine::Renderer {

//=================================================================================
bool C_TrimeshModel::Reload()
{
	return false;
}

//=================================================================================
bool C_TrimeshModel::Load(const std::filesystem::path& filepath)
{
	return false;
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
		if (trimesh.GetNumTriangles() > 10) {
			Utils::HighResolutionTimer BVHBuildTime;
			auto& bvh = m_BVHs.emplace_back(new BVH(trimesh));
			bvh->Build();
		
			CORE_LOG(E_Level::Info, E_Context::Render, "BVH trimesh {} build time {}ms", mesh.m_name, BVHBuildTime.getElapsedTimeFromLastQueryMilliseconds());
			trimesh.SetBVH(bvh);
		}
		else
		{
			m_BVHs.push_back(nullptr);
		}
	}
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
	throw std::logic_error("The method or operation is not implemented.");
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
