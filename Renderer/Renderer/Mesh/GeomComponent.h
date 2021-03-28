#pragma once

#include <Renderer/IRenderableComponent.h>
#include <Renderer/Mesh/Geometry.h>
#include <Renderer/RendererApi.h>

#include <Entity/ComponentBuilder.h>

#include <Utils/Parsing/MaterialParser.h>

namespace GLEngine::Renderer {

class RENDERER_API_EXPORT C_GeomComponent : public Renderer::I_RenderableComponent {
public:
	C_GeomComponent(std::shared_ptr<Entity::I_Entity> owner);
	virtual ~C_GeomComponent();
	virtual void SetupGeometry(const MeshData::Mesh& mesh)				 = 0;
	virtual void SetupMaterial(const Utils::Parsing::MaterialData& data) = 0;

	virtual std::string_view GetDebugComponentName() const override;

protected:
};

//=============================================================
class C_GeometryCompBuilder : public Entity::I_ComponenetBuilder {
public:
	virtual ~C_GeometryCompBuilder() = default;
	RENDERER_API_EXPORT virtual std::shared_ptr<Entity::I_Component> Build(const pugi::xml_node& node, std::shared_ptr<Entity::I_Entity> owner) override;

protected:
	virtual std::shared_ptr<C_GeomComponent> ConstructComponent(std::shared_ptr<Entity::I_Entity> owner) const = 0;
};

} // namespace GLEngine::Renderer
