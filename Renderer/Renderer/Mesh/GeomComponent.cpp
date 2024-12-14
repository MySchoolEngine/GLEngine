#include <RendererStdafx.h>

#include <Renderer/Materials/Material.h>
#include <Renderer/Materials/MaterialManager.h>
#include <Renderer/Mesh/GeomComponent.h>

#include <Utils/Parsing/MaterialParser.h>

namespace GLEngine::Renderer {

//=================================================================================
C_GeomComponent::C_GeomComponent(const std::shared_ptr<Entity::I_Entity>& owner)
	: I_RenderableComponent(owner)
{
}

//=================================================================================
C_GeomComponent::~C_GeomComponent() = default;

//=================================================================================
std::string_view C_GeomComponent::GetDebugComponentName() const
{
	return "GeomComponent";
}

//=================================================================================
void C_GeomComponent::SetupMaterial(const ::Utils::Parsing::MaterialData& data)
{
	auto& materialManager = C_MaterialManager::Instance();
	m_Material			  = materialManager.GetMaterial("GeomComponent");
	if (!m_Material)
	{
		m_Material = materialManager.RegisterMaterial(Renderer::C_Material("GeomComponent"));
	}
	m_Material->SetDiffuseColor(data.m_Color);
	m_Material->SetRoughness(data.m_Roughness);
}

//=================================================================================
void C_GeomComponent::DebugDrawGUI()
{
	m_Material->DrawGUI();
}

//=================================================================================
bool C_GeomComponent::HasDebugDrawGUI() const
{
	return true;
}

} // namespace GLEngine::Renderer
