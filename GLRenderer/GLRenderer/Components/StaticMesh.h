#pragma once

#include <Renderer/IRenderableComponent.h>

#include <GLRenderer/Textures/Texture.h>

#include <GLRenderer/GUI/Input/Color.h>
#include <GLRenderer/GUI/Input/Slider.h>

#include <GLRenderer/Mesh/StaticMeshResource.h>

namespace GLEngine {
namespace Renderer::MeshData
{
struct Mesh;
}
namespace GLRenderer {
namespace Shaders
{
class C_ShaderProgram;
}

namespace Components {

class C_StaticMesh : public Renderer::I_RenderableComponent {
public:
	C_StaticMesh(std::string meshFile, std::string_view shader);
	C_StaticMesh(const Renderer::MeshData::Mesh& meshFile, std::string_view shader);
	virtual void PerformDraw() const override;

	void SetColor(glm::vec3&& color) { m_Color.SetValue(std::move(color)); }
	
	void DebugDrawGUI() override;

protected:
	std::string																			m_meshFile;
	std::shared_ptr<Mesh::C_StaticMeshResource>			m_Mesh;
	std::shared_ptr<Shaders::C_ShaderProgram>				m_Shader;
	GUI::Input::C_ColorRBG													m_Color{ "Model color", glm::vec3(1, 0, 0) };
	GUI::Input::C_Slider<float>											m_Roughness{ 0.f, 0.f, 1.f, "Roughness" };
	std::shared_ptr<Textures::C_Texture>						m_RoughnessMap = nullptr;
	std::shared_ptr<Textures::C_Texture>						m_ColorMap = nullptr;
	std::shared_ptr<Textures::C_Texture>						m_NormalMap = nullptr;

	friend class C_StaticMeshBuilder;
};

class C_StaticMeshBuilder : public Entity::I_ComponenetBuilder
{
public:
	virtual std::shared_ptr<Entity::I_Component> Build(const pugi::xml_node& node, std::shared_ptr<Entity::I_Entity> owner) override;
};

}}}