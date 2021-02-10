#include <GLRendererStdafx.h>

#include <GLRenderer/Commands/HACK/DrawStaticMesh.h>
#include <GLRenderer/Commands/HACK/LambdaCommand.h>
#include <GLRenderer/Components/GLGeomComponent.h>
#include <GLRenderer/Shaders/ShaderManager.h>
#include <GLRenderer/Shaders/ShaderProgram.h>
#include <GLRenderer/Textures/Texture.h>
#include <GLRenderer/Textures/TextureManager.h>
#include <GLRenderer/Textures/TextureUnitManager.h>

#include <Renderer/IRenderer.h>

#include <Core/Application.h>

namespace GLEngine::GLRenderer::Components {

//=================================================================================
C_GLGeomComponent::C_GLGeomComponent(std::shared_ptr<Entity::I_Entity> owner)
	: Renderer::C_GeomComponent(owner)
{
	auto& shmgr = Shaders::C_ShaderManager::Instance();
	m_Shader	= shmgr.GetProgram("basic");
}

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
	auto& tmgr	= Textures::C_TextureManager::Instance();
	auto& tm	= Textures::C_TextureUnitManger::Instance();

	tm.BindTextureToUnit(*(tmgr.GetIdentityTexture()), 0);
	tm.BindTextureToUnit(*(tmgr.GetIdentityTexture()), 1);
	tm.BindTextureToUnit(*(tmgr.GetIdentityTexture()), 2);
	shmgr.ActivateShader(m_Shader);


	if (m_ColorMap)
	{
		tm.BindTextureToUnit(*m_ColorMap, 1);
	}
	else
	{
		tm.BindTextureToUnit(*(tmgr.GetIdentityTexture()), 1);
	}

	renderer->AddCommand(std::move(std::make_unique<Commands::HACK::C_LambdaCommand>(
		[&]() {
			const auto modelMatrix = GetComponentModelMatrix();
			m_Shader->SetUniform("modelMatrix", modelMatrix);
			m_Shader->SetUniform("modelColor", m_Color.GetValue());
			m_Shader->SetUniform("roughness", 1);
			m_Shader->SetUniform("roughnessMap", 0);
			m_Shader->SetUniform("colorMap", 1);
			m_Shader->SetUniform("normalMap", 2);
			m_Shader->SetUniform("useNormalMap", false);
		},
		"GLGeomComponent - upload material")));

	renderer->AddCommand(std::move(std::make_unique<Commands::HACK::C_DrawStaticMesh>(m_Mesh)));
}

//=================================================================================
void C_GLGeomComponent::DebugDrawGUI()
{
	m_Color.Draw();
}

//=================================================================================
bool C_GLGeomComponent::HasDebugDrawGUI() const
{
	return true;
}

//=================================================================================
void C_GLGeomComponent::SetupMaterial(const Utils::Parsing::MaterialData& data)
{
	auto& shmgr = Shaders::C_ShaderManager::Instance();
	m_Shader	= shmgr.GetProgram(data.m_MaterialName);

	auto color = data.m_Color;
	m_Color.SetValue(std::move(color));

	auto& tmgr = Textures::C_TextureManager::Instance();

	if (!data.m_ColorMap.empty())
	{
		m_ColorMap = tmgr.GetTexture(data.m_ColorMap);
		if (m_ColorMap)
		{
			m_Color = glm::vec3(1.0f);

			m_ColorMap->StartGroupOp();
			m_ColorMap->SetWrap(Renderer::E_WrapFunction::Repeat, Renderer::E_WrapFunction::Repeat);
			m_ColorMap->SetFilter(E_OpenGLFilter::LinearMipMapLinear, E_OpenGLFilter::Linear);
			m_ColorMap->GenerateMipMaps();

			m_ColorMap->EndGroupOp();
		}
	}
}

//=================================================================================
Physics::Primitives::S_AABB C_GLGeomComponent::GetAABB() const
{
	return m_AABB;
}

//=================================================================================
std::shared_ptr<Renderer::C_GeomComponent> C_GLGeomComponentBuilder::ConstructComponent(std::shared_ptr<Entity::I_Entity> owner) const
{
	return std::make_shared<C_GLGeomComponent>(owner);
}

} // namespace GLEngine::GLRenderer::Components
