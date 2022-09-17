#pragma once

#include <GLRenderer/Mesh/StaticMeshResource.h>
#include <GLRenderer/Textures/Texture.h>

#include <Renderer/IRenderableComponent.h>
#include <Renderer/Mesh/Loading/MeshResource.h>

#include <GUI/Input/Color.h>
#include <GUI/Input/Slider.h>

#include <rttr/registration.h>
#include <rttr/registration_friend.h>

#include <Core/Resources/ResourceHandle.h>

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
namespace Textures {
class C_Texture;
}
namespace Mesh {
class C_StaticMeshResource;
}

namespace Components {
class C_StaticMesh : public Renderer::I_RenderableComponent {
public:
	C_StaticMesh(std::string meshFile, std::string_view shader, std::shared_ptr<Entity::I_Entity> owner);
	C_StaticMesh();
	C_StaticMesh(Core::ResourceHandle<Renderer::MeshResource> meshHandle,
				 std::string_view							  shader,
				 std::shared_ptr<Entity::I_Entity>			  owner,
				 const Renderer::MeshData::Material*		  material = nullptr);
	virtual void									  PerformDraw() const override;
	[[nodiscard]] virtual Physics::Primitives::S_AABB GetAABB() const override;

	void					 DebugDrawGUI() override;
	virtual std::string_view GetDebugComponentName() const override;
	virtual bool			 HasDebugDrawGUI() const override;

	void		SetShader(const std::string shader);
	std::string GetShader() const;
	void		SetShadowShader(const std::string shader);
	std::string GetShadowShader() const;

	void SetMaterial(std::shared_ptr<Renderer::C_Material> material);

	virtual void Update() override;

	RTTR_ENABLE(Renderer::I_RenderableComponent);

protected:
	void SetMaterial(const Renderer::MeshData::Material& material);

	std::string												 m_meshFile;
	std::vector<std::shared_ptr<Mesh::C_StaticMeshResource>> m_Mesh;
	Core::ResourceHandle<Renderer::MeshResource>			 m_MeshResource;
	std::shared_ptr<Shaders::C_ShaderProgram>				 m_Shader;
	std::shared_ptr<Shaders::C_ShaderProgram>				 m_ShadowPassShader;
	std::shared_ptr<Renderer::C_Material>					 m_Material;
	Physics::Primitives::S_AABB								 m_AABB;

	friend class C_StaticMeshBuilder;
	RTTR_REGISTRATION_FRIEND;
};

class C_StaticMeshBuilder : public Entity::I_ComponenetBuilder {
public:
	virtual std::shared_ptr<Entity::I_Component> Build(const pugi::xml_node& node, std::shared_ptr<Entity::I_Entity> owner) override;
	RTTR_ENABLE(Entity::I_ComponenetBuilder);
};

} // namespace Components
} // namespace GLRenderer
} // namespace GLEngine