#include <RendererStdafx.h>

#include <Renderer/Mesh/Loading/MeshResource.h>
#include <Renderer/Mesh/Loading/ModelLoader.h>
#include <Renderer/Mesh/Scene.h>


namespace GLEngine::Renderer {

//=================================================================================
bool MeshResource::Load(const std::filesystem::path& filepath)
{
	// This does not have to be shared ptr
	m_Scene	   = std::make_shared<Renderer::MeshData::Scene>();
	m_Filepath = filepath;
	Mesh::ModelLoader ml;
	std::lock_guard	  lock(ml.GetMutex());
	ml.Reset();
	return ml.addModelFromFileToScene(m_Filepath, m_Scene, m_TexuterNames);
}

//=================================================================================
bool MeshResource::Reload()
{
	Mesh::ModelLoader ml;
	std::lock_guard	  lock(ml.GetMutex());
	ml.Reset();
	auto newScene = std::make_shared<Renderer::MeshData::Scene>();
	std::vector<std::filesystem::path> newTextures;
	if (ml.addModelFromFileToScene(m_Filepath, newScene, newTextures)) {
		m_Scene = newScene;
		m_TexuterNames = newTextures;
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
std::filesystem::path MeshResource::GetFilePath() const
{
	return m_Filepath;
}

//=================================================================================
const std::vector<std::filesystem::path>& MeshResource::GetTextureNames() const
{
	GLE_ASSERT(IsReady(), "Dereferencing unloaded resource.");
	return m_TexuterNames;
}

//=================================================================================
std::shared_ptr<GLEngine::Core::Resource> MeshLoader::CreateResource() const
{
	return std::make_shared<MeshResource>();
}

//=================================================================================
std::vector<std::string> MeshLoader::GetSupportedExtensions() const
{
	return {".obj", ".dae"};
}

} // namespace GLEngine::Renderer