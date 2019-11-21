#include <GLRendererStdAfx.h>

#include <GLRenderer/Components/SkeletalMesh.h>

namespace GLEngine::GLRenderer::Components {

//=================================================================================
void C_SkeletalMesh::DebugDrawGUI()
{
	std::function<void(const Renderer::Animation::S_Joint)> DrawJointGUI;
	DrawJointGUI = [&DrawJointGUI](const Renderer::Animation::S_Joint& joint)
	{
		if (::ImGui::CollapsingHeader(joint.m_Name.c_str())) {
			auto& pos = joint.GetAnimatedTransform() * glm::vec4(0.f, 0.f, .0f, 1.f);
			::ImGui::Text("Original pos: [%f, %f, %f]", pos.x, pos.y, pos.z);
			for (const auto& child : joint.m_Children)
			{
				DrawJointGUI(child);
			}
		}
	};
	if (::ImGui::CollapsingHeader("Skeleton")) {
		DrawJointGUI(*(m_Skeleton.m_Root.get()));
	}
}

//=================================================================================
void C_SkeletalMesh::PerformDraw() const
{

}

//=================================================================================
void C_SkeletalMesh::Update()
{
	C_DebugDraw::Instance().DrawSkeleton(glm::vec3(1.0f, .0f, .0f), m_Skeleton);
}

//=================================================================================
C_SkeletalMesh::C_SkeletalMesh()
{
	m_Skeleton.m_Root = std::make_unique<Renderer::Animation::S_Joint>(0, "root", glm::translate(glm::mat4(1.0f), glm::vec3(0, 1, 0)));
	m_Skeleton.m_Root->m_Children.emplace_back(0, "child", glm::translate(glm::mat4(1.0f), glm::vec3(0, 1, 1)));

	m_Skeleton.InitializeSkeleton();
}

}
