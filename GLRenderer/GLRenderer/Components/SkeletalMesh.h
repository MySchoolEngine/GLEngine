#pragma once

#include <GLRenderer/Mesh/StaticMeshResource.h>
#include <GLRenderer/Textures/Texture.h>

#include <Renderer/Animation/SkeletalAnimation.h>
#include <Renderer/Animation/Skeleton.h>
#include <Renderer/IRenderableComponent.h>

#include <GUI/Input/CheckBoxValue.h>
#include <GUI/Input/Slider.h>

#include <Entity/ComponentBuilder.h>

namespace GLEngine::GLRenderer::Buffers::UBO {
class C_JointTramsformsUBO;
}

namespace GLEngine::GLRenderer::Components {

class C_SkeletalMesh : public Renderer::I_RenderableComponent {
public:
	C_SkeletalMesh(std::shared_ptr<Entity::I_Entity> owner, std::string meshFile, std::string meshFolder = "Models");
	virtual void PerformDraw() const override;
	virtual void Update() override;

	virtual void									  DebugDrawGUI() override;
	virtual bool									  HasDebugDrawGUI() const override;
	virtual std::string_view						  GetDebugComponentName() const override;
	[[nodiscard]] virtual Physics::Primitives::S_AABB GetAABB() const override;

public:
	std::shared_ptr<Mesh::C_StaticMeshResource>			m_Mesh;
	std::shared_ptr<Textures::C_Texture>				m_Texture;
	Renderer::Animation::C_Skeleton						m_Skeleton;
	Renderer::Animation::C_SkeletalAnimation			m_Animation;
	std::shared_ptr<Buffers::UBO::C_JointTramsformsUBO> m_TransformationUBO;
	GUI::Input::C_CheckBoxValue							m_RenderMesh;
	GUI::Input::C_Slider<float>							m_AnimationProgress;
	Physics::Primitives::S_AABB							m_AABB;

	VAO::C_GLVAO<5> m_VAO;
	std::size_t		m_triangles;
	glm::mat4		m_ModelMatrix;
};

//=============================================================
class C_SkeletalMeshBuilder : public Entity::I_ComponenetBuilder {
public:
	virtual std::shared_ptr<Entity::I_Component> Build(const pugi::xml_node& node, std::shared_ptr<Entity::I_Entity> owner) override;
};

} // namespace GLEngine::GLRenderer::Components