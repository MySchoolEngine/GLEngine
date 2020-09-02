#pragma once

#include <GLRenderer/Mesh/StaticMeshResource.h>
#include <GLRenderer/Textures/Texture.h>

#include <Renderer/Mesh/GeomComponent.h>
#include <GUI/Input/Color.h>

namespace GLEngine::GLRenderer::Shaders
{
class C_ShaderProgram;
}

namespace GLEngine::GLRenderer::Components {

class C_GLGeomComponent : public Renderer::C_GeomComponent
{
public:
	explicit C_GLGeomComponent(std::shared_ptr<Entity::I_Entity> owner);

	//=================================================================================
	void SetupMaterial(const Utils::Parsing::MaterialData& data) override;
	void SetupGeometry(const Renderer::MeshData::Mesh& mesh) override;
	void PerformDraw() const override;
	void DebugDrawGUI() override;
	[[nodiscard]] virtual Physics::Primitives::S_AABB GetAABB() const override;

private:
	std::shared_ptr<Mesh::C_StaticMeshResource>			m_Mesh;
	std::shared_ptr<Shaders::C_ShaderProgram>				m_Shader;
	GUI::Input::C_ColorRBG													m_Color{ "Model color", glm::vec3(1, 0, 0) };
	std::shared_ptr<Textures::C_Texture>						m_ColorMap = nullptr;
	Physics::Primitives::S_AABB											m_AABB;
};

//=================================================================================
class C_GLGeomComponentBuilder : public Renderer::C_GeometryCompBuilder
{
protected:
	//=================================================================================
	virtual std::shared_ptr<Renderer::C_GeomComponent> ConstructComponent(std::shared_ptr<Entity::I_Entity> owner) const override;

};
}
