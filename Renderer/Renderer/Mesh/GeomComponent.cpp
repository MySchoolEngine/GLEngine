#include <RendererStdafx.h>

#include <Renderer/Materials/Material.h>
#include <Renderer/Materials/MaterialManager.h>
#include <Renderer/Mesh/GeomComponent.h>
#include <Renderer/Mesh/Geometry.h>

#include <Utils/Parsing/MatrixParse.h>

#include <pugixml.hpp>

namespace GLEngine::Renderer {

//=================================================================================
C_GeomComponent::C_GeomComponent(std::shared_ptr<Entity::I_Entity> owner)
	: I_RenderableComponent(owner)
{
}

//=================================================================================
std::string_view C_GeomComponent::GetDebugComponentName() const
{
	return "GeomComponent";
}

//=================================================================================
void C_GeomComponent::SetupMaterial(const Utils::Parsing::MaterialData& data)
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
C_GeomComponent::~C_GeomComponent() = default;

//=================================================================================
C_GeometryCompBuilder::~C_GeometryCompBuilder() = default;

//=================================================================================
std::shared_ptr<Entity::I_Component> C_GeometryCompBuilder::Build(const pugi::xml_node& node, std::shared_ptr<Entity::I_Entity> owner)
{
	auto	   geomComponent = ConstructComponent(owner);
	const auto material		 = Utils::Parsing::C_MaterialParser::ParseMaterialData(node);
	geomComponent->SetupMaterial(material);
	if (/*const auto typeAttribute = */ node.attribute("type"))
	{
		int subdivisions = 0;
		if (const auto subdivisionsAtt = node.attribute("subdivisions"))
		{
			subdivisions = subdivisionsAtt.as_int();
		}
		geomComponent->SetupGeometry(MeshData::C_Geometry::CreatePlane(subdivisions));
	}
	else
	{
		CORE_LOG(E_Level::Warning, E_Context::Render, "Geomtery component needs type attribute");
	}
	const auto transfomr = Utils::Parsing::C_MatrixParser::ParseTransformation(node);
	const auto rotation	 = Utils::Parsing::C_MatrixParser::ParseRotations(node);
	const auto scale	 = Utils::Parsing::C_MatrixParser::ParseScale(node);

	geomComponent->SetComponentMatrix(transfomr * rotation * scale);

	return geomComponent;
}

} // namespace GLEngine::Renderer
