#pragma once

#include <Renderer/Mesh/GeomComponent.h>

namespace GLEngine::GLRenderer {
namespace Shaders
{
class C_ShaderProgram;
}
namespace Textures
{
class C_Texture;
}
namespace Mesh
{
class C_StaticMeshResource;
}

namespace Components {

class C_GLGeomComponent : public Renderer::C_GeomComponent
{
public:
	explicit C_GLGeomComponent(std::shared_ptr<Entity::I_Entity> owner);

	//=================================================================================
	void SetupMaterial(const Utils::Parsing::MaterialData& data) override;
	void SetupGeometry(const Renderer::MeshData::Mesh& mesh) override;
	void PerformDraw() const override;
	void DebugDrawGUI() override;
	[[nodiscard]] virtual const Physics::Primitives::S_AABB& GetAABB() const override;

private:
	std::shared_ptr<Mesh::C_StaticMeshResource>	m_Mesh;
	std::shared_ptr<Shaders::C_ShaderProgram>	m_Shader;
	std::shared_ptr<Textures::C_Texture>		m_ColorMap = nullptr;
	Physics::Primitives::S_AABB					m_AABB;
};

//=================================================================================
class C_GLGeomComponentBuilder : public Renderer::C_GeometryCompBuilder
{
protected:
	//=================================================================================
	virtual std::shared_ptr<Renderer::C_GeomComponent> ConstructComponent(std::shared_ptr<Entity::I_Entity> owner) const override;
};

}}
