#pragma once

#include <Renderer/IRenderableComponent.h>
#include <Renderer/Materials/MaterialResource.h>
#include <Renderer/Mesh/Loading/MeshResource.h>
#include <Renderer/Renderer3D.h>
#include <Renderer/RendererApi.h>
#include <Renderer/Resources/RenderResourceHandle.h>

#include <Core/Resources/ResourceHandle.h>

#include <rttr/registration_friend.h>

namespace GLEngine::Renderer {

class C_Material;

class RENDERER_API_EXPORT C_StaticMeshHandles : public I_RenderableComponent {
public:
	C_StaticMeshHandles();
	~C_StaticMeshHandles() override = default;

	void						 PerformDraw() const override;
	bool						 HasDebugDrawGUI() const override;
	std::string_view			 GetDebugComponentName() const override;
	Physics::Primitives::S_AABB	 GetAABB() const override;
	void						 OnEvent(Core::I_Event& event) override;
	void						 Update() override;
	void						 Render(Renderer3D& renderer) const;
	void						 SetMeshFile(const std::filesystem::path& meshFile);
	const std::filesystem::path& GetMeshFile() const;
	[[nodiscard]] bool			 IsMeshReady() const;
	void						 DebugDrawGUI() override;

	RTTR_ENABLE(Renderer::I_RenderableComponent);


private:
	void CleanRenderData();
	void CopyMaterialsFromSourceFile();

	// this will be removed once material manager uses handles
	std::shared_ptr<C_Material> ConvertHandleToMaterial(const Core::ResourceHandle<MaterialResource>& handle) const;
	void						ConvertHandlesToMaterials();

	struct MeshContainer {
		Handle<Buffer> m_PositionsHandle;
		Handle<Buffer> m_NormalsHandle;
		Handle<Buffer> m_TexCoordsHandle;
		Handle<Buffer> m_TangentHandle;
		Handle<Buffer> m_BitangentHandle;
		uint32_t	   m_NumPrimitives;
	};
	Core::ResourceHandle<MeshResource>					m_MeshResource;
	Core::ResourceHandle<MeshResource>					m_MeshResourceLive;
	std::vector<Core::ResourceHandle<MaterialResource>> m_MaterialHandles;
	std::vector<MeshContainer>							m_Meshes;
	Handle<Pipeline>									m_Pipeline;
	std::vector<std::shared_ptr<C_Material>>			m_Materials;

	RTTR_REGISTRATION_FRIEND;
};
} // namespace GLEngine::Renderer
