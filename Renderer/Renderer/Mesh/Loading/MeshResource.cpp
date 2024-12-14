#include <RendererStdafx.h>

#include <Renderer/Mesh/Loading/MeshResource.h>
#include <Renderer/Mesh/Loading/ModelLoader.h>
#include <Renderer/Mesh/Scene.h>

#include <Core/Resources/ResourceHandle.h>
#include <Core/Resources/ResourceManager.h>

#include <rttr/registration>

namespace GLEngine::Core {
template <> void ResourceHandle<Renderer::MeshResource>::AfterDeserialize(GLEngine::Utils::C_XMLDeserializer::DeserializeCtx& ctx)
{
	auto& rm = C_ResourceManager::Instance();
	*this	 = rm.LoadResource<Renderer::MeshResource>(GetFilePath());
}
} // namespace GLEngine::Core
DECLARE_RESOURCE_TYPE(GLEngine::Renderer::MeshResource)

namespace GLEngine::Renderer {


//=================================================================================
MeshResource::MeshResource() = default;

//=================================================================================
bool MeshResource::Load(const std::filesystem::path& filepath)
{
	// This does not have to be shared ptr
	m_Scene	   = std::make_shared<MeshData::Scene>();
	m_Filepath = filepath;
	Mesh::ModelLoader ml;
	std::lock_guard	  lock(ml.GetMutex());
	ml.Reset();
	return ml.addModelFromFileToScene(m_Filepath, m_Scene, m_TextureNames);
}

//=================================================================================
bool MeshResource::Reload()
{
	Mesh::ModelLoader ml;
	std::lock_guard	  lock(ml.GetMutex());
	ml.Reset();
	const auto						   newScene = std::make_shared<MeshData::Scene>();
	std::vector<std::filesystem::path> newTextures;
	if (ml.addModelFromFileToScene(m_Filepath, newScene, newTextures))
	{
		m_Scene		   = newScene;
		m_TextureNames = newTextures;
		return true;
	}

	CORE_LOG(E_Level::Warning, E_Context::Render, "Unable to reload mesh {}", m_Filepath);
	return false;
}

//=================================================================================
MeshData::Scene& MeshResource::GetScene()
{
	GLE_ASSERT(IsReady(), "Dereferencing unloaded resource.");
	return *m_Scene.get();
}

//=================================================================================
const MeshData::Scene& MeshResource::GetScene() const
{
	GLE_ASSERT(IsReady(), "Dereferencing unloaded resource.");
	return *m_Scene.get();
}

//=================================================================================
const std::vector<std::filesystem::path>& MeshResource::GetTextureNames() const
{
	GLE_ASSERT(IsReady(), "Dereferencing unloaded resource.");
	return m_TextureNames;
}

//=================================================================================
std::unique_ptr<Core::I_ResourceLoader> MeshResource::GetLoader()
{
	return std::make_unique<MeshLoader>();
}

//=================================================================================
std::shared_ptr<Core::Resource> MeshLoader::CreateResource() const
{
	return std::make_shared<MeshResource>();
}

//=================================================================================
std::vector<std::string> MeshLoader::GetSupportedExtensions() const
{
	return {".obj", ".dae"};
}

} // namespace GLEngine::Renderer