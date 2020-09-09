#pragma once

#include <Renderer/RendererApi.h>

#include <Renderer/IRenderableComponent.h>
#include <Renderer/Mesh/Geometry.h>
#include <Renderer/Materials/Material.h>

#include <Entity/IComponent.h>

#include <Utils/Parsing/MaterialParser.h>

namespace GLEngine::Renderer {

class C_Material;

class RENDERER_API_EXPORT C_GeomComponent : public I_RenderableComponent
{
public:
	C_GeomComponent(std::shared_ptr<Entity::I_Entity> owner);
	virtual ~C_GeomComponent();
	virtual void SetupGeometry(const MeshData::Mesh& mesh) = 0;
	virtual void SetupMaterial(const Utils::Parsing::MaterialData& data);
protected:
	std::shared_ptr<C_Material> m_Material;
};

//=============================================================
class C_GeometryCompBuilder : public Entity::I_ComponenetBuilder
{
public:
	virtual ~C_GeometryCompBuilder() = default;
	RENDERER_API_EXPORT virtual std::shared_ptr<Entity::I_Component> Build(const pugi::xml_node& node, std::shared_ptr<Entity::I_Entity> owner) override;
protected:
	virtual std::shared_ptr<C_GeomComponent> ConstructComponent(std::shared_ptr<Entity::I_Entity> owner) const = 0;
};

}

