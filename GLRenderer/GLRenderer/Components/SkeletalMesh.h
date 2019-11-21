#pragma once

#include <Renderer/IRenderableComponent.h>

#include <Renderer/Animation/Skeleton.h>

namespace GLEngine::GLRenderer::Components {

class C_SkeletalMesh : public Renderer::I_RenderableComponent {
public:
	C_SkeletalMesh();
	virtual void PerformDraw() const override;
	virtual void Update() override;

	virtual void DebugDrawGUI() override;
public:
	Renderer::Animation::C_Skeleton m_Skeleton;
};

}