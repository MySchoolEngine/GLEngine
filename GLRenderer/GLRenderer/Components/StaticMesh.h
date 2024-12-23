#pragma once

#include <GLRenderer/Mesh/StaticMeshResource.h>

#include <Renderer/IRenderableComponent.h>
#include <Renderer/Mesh/Loading/MeshResource.h>

#include <Core/Resources/ResourceHandle.h>

#include <rttr/registration.h>
#include <rttr/registration_friend.h>

namespace GLEngine {
namespace Renderer::MeshData {
struct Mesh;
}
namespace Renderer {
class C_Material;
}

namespace GLRenderer {
namespace Shaders {
class C_ShaderProgram;
}
namespace Mesh {
class C_StaticMeshResource;
}

namespace Components {
class C_StaticMesh : public Renderer::I_RenderableComponent {
public:
	C_StaticMesh(std::string meshFile, std::string_view shader, const std::shared_ptr<Entity::I_Entity>& owner);
	C_StaticMesh();
	C_StaticMesh(const Core::ResourceHandle<Renderer::MeshResource>& meshHandle,
				 std::string_view							  shader,
				 const std::shared_ptr<Entity::I_Entity>&			 owner,
				 const Renderer::MeshData::Material*		  material = nullptr);
	~C_StaticMesh();
	virtual void									  PerformDraw() const override;
	[[nodiscard]] virtual Physics::Primitives::S_AABB GetAABB() const override;

	void					 DebugDrawGUI() override;
	virtual std::string_view GetDebugComponentName() const override;
	virtual bool			 HasDebugDrawGUI() const override;

	void				  SetShader(const std::string shader);
	std::string			  GetShader() const;
	void				  SetShadowShader(const std::string shader);
	std::string			  GetShadowShader() const;
	void				  SetMeshFile(const std::filesystem::path meshfile);
	std::filesystem::path GetMeshFile() const;

	void SetMaterial(const std::shared_ptr<Renderer::C_Material>& material);

	virtual void Update() override;

	RTTR_ENABLE(Renderer::I_RenderableComponent);

protected:
	void SetMaterial(const Renderer::MeshData::Material& material);

	std::vector<std::shared_ptr<Mesh::C_StaticMeshResource>> m_Mesh;
	Core::ResourceHandle<Renderer::MeshResource>			 m_MeshResource;
	std::shared_ptr<Shaders::C_ShaderProgram>				 m_Shader;
	std::shared_ptr<Shaders::C_ShaderProgram>				 m_ShadowPassShader;
	std::shared_ptr<Renderer::C_Material>					 m_Material;
	Physics::Primitives::S_AABB								 m_AABB;

	RTTR_REGISTRATION_FRIEND;
};

} // namespace Components
} // namespace GLRenderer
} // namespace GLEngine