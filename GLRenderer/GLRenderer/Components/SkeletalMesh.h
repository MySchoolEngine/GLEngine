#pragma once

#include <Renderer/IRenderableComponent.h>
#include <Renderer/Animation/Skeleton.h>


#include <GLRenderer/Mesh/StaticMeshResource.h>

#include <GLRenderer/Textures/Texture.h>
#include <GLRenderer/GUI/Input/CheckBoxValue.h>

#include <Renderer/Animation/SkeletalAnimation.h>

namespace GLEngine::GLRenderer::Components {

class C_SkeletalMesh : public Renderer::I_RenderableComponent {
public:
	C_SkeletalMesh(std::string meshFile, std::string meshFolder = "Models");
	virtual void PerformDraw() const override;
	virtual void Update() override;

	virtual void DebugDrawGUI() override;
public:
	std::shared_ptr<Mesh::C_StaticMeshResource>		m_Mesh;
	std::shared_ptr<Textures::C_Texture>					m_Texture;
	Renderer::Animation::C_Skeleton								m_Skeleton;
	Renderer::Animation::C_SkeletalAnimation			m_Animation;
	GUI::Input::C_CheckBoxValue										m_RenderMesh;
};

}