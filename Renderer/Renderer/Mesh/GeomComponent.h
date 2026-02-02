#pragma once

#include <Renderer/IRenderableComponent.h>
#include <Renderer/Materials/Material.h>
#include <Renderer/Mesh/Scene.h>
#include <Renderer/RendererApi.h>

#include <Utils/Parsing/MaterialParser.h>

namespace GLEngine::Renderer {
class Renderer3D;
class C_Material;

class RENDERER_API_EXPORT C_GeomComponent : public I_RenderableComponent {
public:
	C_GeomComponent();
	C_GeomComponent(const std::shared_ptr<Entity::I_Entity>& owner);
	~C_GeomComponent() override;

	void						Update() override;
	void						SetupGeometry(const MeshData::Mesh& mesh);
	void						Render(Renderer3D& renderer) const;
	void						PerformDraw() const override {}
	Physics::Primitives::S_AABB GetAABB() const override;

	void			   AfterDeserialize(Utils::C_XMLDeserializer::DeserializeCtx& ctx);
	void			   DebugDrawGUI() override;
	[[nodiscard]] bool HasDebugDrawGUI() const override;

	std::string_view GetDebugComponentName() const override;

	RTTR_ENABLE(Renderer::I_RenderableComponent)
protected:
	struct MeshContainer {
		Handle<Buffer> m_PositionsHandle;
		Handle<Buffer> m_NormalsHandle;
		Handle<Buffer> m_TexCoordsHandle;
		Handle<Buffer> m_TangentHandle;
		Handle<Buffer> m_BitangentHandle;
		uint32_t	   m_NumPrimitives;
	};
	MeshContainer				m_Mesh;
	Handle<Pipeline>			m_Pipeline;
	std::shared_ptr<C_Material> m_Material;
	Physics::Primitives::S_AABB m_AABB;

	RTTR_REGISTRATION_FRIEND
};

} // namespace GLEngine::Renderer
