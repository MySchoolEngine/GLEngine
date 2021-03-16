#include <RendererStdafx.h>

#include <Renderer/Mesh/GeomComponent.h>

#include <Utils/Parsing/MaterialParser.h>
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
C_GeomComponent::~C_GeomComponent() = default;


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
		CORE_LOG(E_Level::Warning, E_Context::Render, "Geomtery componen needs type attribute");
	}
	const auto transfomr = Utils::Parsing::C_MatrixParser::ParseTransformation(node);
	const auto rotation	 = Utils::Parsing::C_MatrixParser::ParseRotations(node);
	const auto scale	 = Utils::Parsing::C_MatrixParser::ParseScale(node);

	geomComponent->SetComponentMatrix(transfomr * rotation * scale);

	return geomComponent;
}

} // namespace GLEngine::Renderer
