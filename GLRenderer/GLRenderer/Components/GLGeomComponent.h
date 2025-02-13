#pragma once

#include <Renderer/Mesh/GeomComponent.h>

#include <GUI/Input/Color.h>

namespace GLEngine::GLRenderer {
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

class C_GLGeomComponent : public Renderer::C_GeomComponent {
public:
	explicit C_GLGeomComponent(std::shared_ptr<Entity::I_Entity> owner);
	~C_GLGeomComponent() override;
	//=================================================================================
	void									  SetupMaterial(const ::Utils::Parsing::MaterialData& data) override;
	void									  SetupGeometry(const Renderer::MeshData::Mesh& mesh) override;
	void									  PerformDraw() const override;
	[[nodiscard]] Physics::Primitives::S_AABB GetAABB() const override;

private:
	std::shared_ptr<Mesh::C_StaticMeshResource> m_Mesh;
	std::shared_ptr<Shaders::C_ShaderProgram>	m_Shader;
	Physics::Primitives::S_AABB					m_AABB;
};
} // namespace Components

} // namespace GLEngine::GLRenderer
