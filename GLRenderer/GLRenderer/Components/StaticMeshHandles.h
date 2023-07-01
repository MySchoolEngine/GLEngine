#pragma once

#include <Renderer/IRenderableComponent.h>
#include <Renderer/Mesh/Loading/MeshResource.h>
#include <Renderer/Renderer3D.h>
#include <Renderer/Resources/RenderResourceHandle.h>

#include <Core/Resources/ResourceHandle.h>

#include <rttr/registration.h>
#include <rttr/registration_friend.h>

namespace GLEngine::GLRenderer {

class C_StaticMeshHandles : public Renderer::I_RenderableComponent {
public:
	C_StaticMeshHandles();
	virtual ~C_StaticMeshHandles() = default;

	virtual void						PerformDraw() const override;
	virtual bool						HasDebugDrawGUI() const override;
	virtual std::string_view			GetDebugComponentName() const override;
	virtual Physics::Primitives::S_AABB GetAABB() const override;
	virtual void						OnEvent(Core::I_Event& event) override;
	virtual void						Update() override;
	void								Render(Renderer::Renderer3D& renderer) const;

private:
	struct MeshContainer {
		Renderer::Handle<Renderer::Buffer> m_PositionsHandle;
		Renderer::Handle<Renderer::Buffer> m_NormalsHandle;
		Renderer::Handle<Renderer::Buffer> m_TexCoordsHandle;
		Renderer::Handle<Renderer::Buffer> m_TangentHandle;
		Renderer::Handle<Renderer::Buffer> m_BitangentHandle;
	};
	Core::ResourceHandle<Renderer::MeshResource> m_MeshResource;
	std::vector<MeshContainer>					 m_Meshes;
};
} // namespace GLEngine::GLRenderer
