#pragma once

#include <Renderer/IRenderableComponent.h>
#include <Renderer/Materials/Material.h>
#include <Renderer/Mesh/Scene.h>
#include <Renderer/RendererApi.h>

#include <Entity/IComponent.h>

#include <Utils/Parsing/MaterialParser.h>

namespace GLEngine::Renderer {

class C_Material;

class RENDERER_API_EXPORT C_GeomComponent : public I_RenderableComponent {
public:
	C_GeomComponent(std::shared_ptr<Entity::I_Entity> owner);
	virtual ~C_GeomComponent();
	virtual void SetupGeometry(const MeshData::Mesh& mesh)				 = 0;
	virtual void SetupMaterial(const ::Utils::Parsing::MaterialData& data) = 0;

	virtual void			   DebugDrawGUI() override;
	[[nodiscard]] virtual bool HasDebugDrawGUI() const override;

	virtual std::string_view GetDebugComponentName() const override;

protected:
	std::shared_ptr<C_Material> m_Material;
};

} // namespace GLEngine::Renderer
