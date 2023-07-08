#pragma once

#include <Renderer/RendererApi.h>

#include <Renderer/IRenderableComponent.h>
#include <Renderer/Mesh/Loading/MeshResource.h>
#include <Renderer/Renderer3D.h>
#include <Renderer/Resources/RenderResourceHandle.h>

#include <Core/Resources/ResourceHandle.h>

#include <rttr/registration.h>
#include <rttr/registration_friend.h>

namespace GLEngine::Renderer {

class RENDERER_API_EXPORT C_StaticMeshHandles : public I_RenderableComponent {
public:
	C_StaticMeshHandles();
	virtual ~C_StaticMeshHandles() = default;

	virtual void						PerformDraw() const override;
	virtual bool						HasDebugDrawGUI() const override;
	virtual std::string_view			GetDebugComponentName() const override;
	virtual Physics::Primitives::S_AABB GetAABB() const override;
	virtual void						OnEvent(Core::I_Event& event) override;
	virtual void						Update() override;
	void								Render(Renderer3D& renderer) const;

private:
	struct MeshContainer {
		Handle<Buffer> m_PositionsHandle;
		Handle<Buffer> m_NormalsHandle;
		Handle<Buffer> m_TexCoordsHandle;
		Handle<Buffer> m_TangentHandle;
		Handle<Buffer> m_BitangentHandle;
		uint32_t	   m_NumPrimitives;
	};
	Core::ResourceHandle<MeshResource> m_MeshResource;
	std::vector<MeshContainer>		   m_Meshes;
};
} // namespace GLEngine::Renderer
