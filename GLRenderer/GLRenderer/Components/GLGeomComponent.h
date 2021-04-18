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
	virtual ~C_GLGeomComponent();
	//=================================================================================
	void											  SetupMaterial(const Utils::Parsing::MaterialData& data) override;
	void											  SetupGeometry(const Renderer::MeshData::Mesh& mesh) override;
	void											  PerformDraw() const override;
	void											  DebugDrawGUI(GUI::C_GUIManager* guiMGR = nullptr) override;
	[[nodiscard]] virtual bool						  HasDebugDrawGUI() const override;
	[[nodiscard]] virtual Physics::Primitives::S_AABB GetAABB() const override;

private:
	std::shared_ptr<Mesh::C_StaticMeshResource> m_Mesh;
	std::shared_ptr<Shaders::C_ShaderProgram>	m_Shader;
	GUI::Input::C_ColorRBG						m_Color{"Model color", glm::vec3(1, 0, 0)};
	std::shared_ptr<Textures::C_Texture>		m_ColorMap = nullptr;
	Physics::Primitives::S_AABB					m_AABB;
};

//=================================================================================
class C_GLGeomComponentBuilder : public Renderer::C_GeometryCompBuilder {
protected:
	//=================================================================================
	[[nodiscard]] virtual std::shared_ptr<Renderer::C_GeomComponent> ConstructComponent(std::shared_ptr<Entity::I_Entity> owner) const override;
};
} // namespace Components

} // namespace GLEngine::GLRenderer
