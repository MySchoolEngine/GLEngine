#pragma once

#include <GLRenderer/Mesh/StaticMeshResource.h>
#include <GLRenderer/Textures/Texture.h>

#include <Renderer/IRenderableComponent.h>

#include <GUI/Input/Color.h>
#include <GUI/Input/Slider.h>

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
	C_StaticMesh(const Renderer::MeshData::Mesh&	 meshFile,
				 std::string_view					 shader,
				 std::shared_ptr<Entity::I_Entity>	 owner,
				 const Renderer::MeshData::Material* material = nullptr);
	virtual void									  PerformDraw() const override;
	[[nodiscard]] virtual Physics::Primitives::S_AABB GetAABB() const override;

	void SetColor(glm::vec3&& color) { m_Color.SetValue(std::move(color)); }
	void SetColor(const glm::vec3& color)
	{
		auto ccolor = color;
		m_Color.SetValue(std::move(ccolor));
	}

	void					 DebugDrawGUI() override;
	virtual std::string_view GetDebugComponentName() const override;
	virtual bool			 HasDebugDrawGUI() const override;

	void SetColorMap(std::shared_ptr<Textures::C_Texture>& texture);
	void SetRoughnessMap(const std::shared_ptr<Textures::C_Texture>& texture);
	void SetNormalMap(std::shared_ptr<Textures::C_Texture>& texture);

protected:
	void SetMaterial(const Renderer::MeshData::Material& material);

	std::string									m_meshFile;
	std::shared_ptr<Mesh::C_StaticMeshResource> m_Mesh;
	std::shared_ptr<Shaders::C_ShaderProgram>	m_Shader;
	std::shared_ptr<Shaders::C_ShaderProgram>	m_ShadowPassShader;
	std::shared_ptr<Renderer::C_Material>		m_Material;
	GUI::Input::C_ColorRBG						m_Color{"Model color", glm::vec3(1, 0, 0)};
	GUI::Input::C_Slider<float>					m_Roughness{0.f, 0.f, 1.f, "Roughness"};
	std::shared_ptr<Textures::C_Texture>		m_RoughnessMap = nullptr;
	std::shared_ptr<Textures::C_Texture>		m_ColorMap	   = nullptr;
	std::shared_ptr<Textures::C_Texture>		m_NormalMap	   = nullptr;
	Physics::Primitives::S_AABB					m_AABB;

	friend class C_StaticMeshBuilder;
};

class C_StaticMeshBuilder : public Entity::I_ComponenetBuilder {
public:
	virtual std::shared_ptr<Entity::I_Component> Build(const pugi::xml_node& node, std::shared_ptr<Entity::I_Entity> owner) override;
};

} // namespace Components
} // namespace GLRenderer
} // namespace GLEngine