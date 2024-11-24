#include <GLRendererStdafx.h>

#include <GLRenderer/Buffers/UBO/ModelData.h>
#include <GLRenderer/Buffers/UniformBuffersManager.h>
#include <GLRenderer/Commands/HACK/DrawStaticMesh.h>
#include <GLRenderer/Commands/HACK/LambdaCommand.h>
#include <GLRenderer/Components/GLGeomComponent.h>
#include <GLRenderer/Mesh/StaticMeshResource.h>
#include <GLRenderer/Shaders/ShaderManager.h>
#include <GLRenderer/Shaders/ShaderProgram.h>
#include <GLRenderer/Textures/Texture.h>
#include <GLRenderer/Textures/TextureManager.h>

#include <Renderer/IRenderer.h>

#include <Core/Application.h>

#include <imgui.h>

namespace GLEngine::GLRenderer::Components {

//=================================================================================
C_GLGeomComponent::C_GLGeomComponent(std::shared_ptr<Entity::I_Entity> owner)
	: Renderer::C_GeomComponent(owner)
{
	auto& shmgr = Shaders::C_ShaderManager::Instance();
	m_Shader	= shmgr.GetProgram("basic");
}

//=================================================================================
C_GLGeomComponent::~C_GLGeomComponent() = default;

//=================================================================================
void C_GLGeomComponent::SetupGeometry(const Renderer::MeshData::Mesh& mesh)
{
	m_Mesh = std::make_shared<Mesh::C_StaticMeshResource>(mesh);
	m_AABB = mesh.bbox;
}

//=================================================================================
void C_GLGeomComponent::PerformDraw() const
{
	auto& renderer = Core::C_Application::Get().GetActiveRenderer();

	auto& shmgr = Shaders::C_ShaderManager::Instance();
	shmgr.ActivateShader(m_Shader);

	renderer.AddCommand(std::make_unique<Commands::HACK::C_LambdaCommand>(
		[&, matIndex = m_Material ? m_Material->GetMaterialIndex() : 0]() {
			ErrorCheck();
			auto modelData = Buffers::C_UniformBuffersManager::Instance().GetBufferByName("modelData");
			if (auto modelDataUbo = std::dynamic_pointer_cast<Buffers::UBO::C_ModelData>(modelData))
			{
				modelDataUbo->SetModelMatrix(GetComponentModelMatrix());
				modelDataUbo->SetMaterialIndex(matIndex);
				modelDataUbo->UploadData();
			}
			ErrorCheck();
		},
		"GLGeomComponent - upload matrice"));

	renderer.AddCommand(std::make_unique<Commands::HACK::C_DrawStaticMesh>(m_Mesh));
}

//=================================================================================
void C_GLGeomComponent::SetupMaterial(const ::Utils::Parsing::MaterialData& data)
{
	C_GeomComponent::SetupMaterial(data);
	auto& shmgr = Shaders::C_ShaderManager::Instance();
	m_Shader	= shmgr.GetProgram(data.m_MaterialName);

	if (!data.m_ColorMap.empty())
	{
		m_Material->SetColorMapPath(data.m_ColorMap);
	}
}

//=================================================================================
Physics::Primitives::S_AABB C_GLGeomComponent::GetAABB() const
{
	return m_AABB;
}

} // namespace GLEngine::GLRenderer::Components
