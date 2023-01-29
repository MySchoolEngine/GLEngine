#pragma once

#include <GLRenderer/VAO/VAO.h>

#include <Renderer/Animation/SkeletalAnimation.h>
#include <Renderer/Animation/Skeleton.h>
#include <Renderer/IRenderableComponent.h>
#include <Renderer/Textures/DeviceTexture.h>
#include <Renderer/Textures/TextureResource.h>

#include <GUI/Input/CheckBoxValue.h>
#include <GUI/Input/Slider.h>
#include <GUI/Texture.h>

#include <Core/Resources/ResourceHandle.h>

namespace GLEngine::GLRenderer::Buffers::UBO {
class C_JointTramsformsUBO;
} // namespace GLEngine::GLRenderer::Buffers::UBO

namespace GLEngine::GLRenderer::Components {

class C_SkeletalMesh : public Renderer::I_RenderableComponent {
public:
	C_SkeletalMesh(std::shared_ptr<Entity::I_Entity> owner, const std::filesystem::path& meshFile, const std::filesystem::path& meshFolder = "Models");
	~C_SkeletalMesh();
	virtual void PerformDraw() const override;
	virtual void Update() override;

	virtual void									  DebugDrawGUI() override;
	virtual bool									  HasDebugDrawGUI() const override;
	virtual std::string_view						  GetDebugComponentName() const override;
	[[nodiscard]] virtual Physics::Primitives::S_AABB GetAABB() const override;

	void						 SetColorMapPath(const std::filesystem::path& path);
	const std::filesystem::path& GetColorMapPath() const;

public:
	Core::ResourceHandle<Renderer::TextureResource>		m_ColorMapRes;
	std::shared_ptr<Renderer::I_DeviceTexture>			m_ColorMap;
	GUI::C_Texture										m_ColorMapGUI;
	Renderer::C_Skeleton								m_Skeleton;
	Renderer::C_SkeletalAnimation						m_Animation;
	std::shared_ptr<Buffers::UBO::C_JointTramsformsUBO> m_TransformationUBO;
	GUI::Input::C_CheckBoxValue							m_RenderMesh;
	GUI::Input::C_CheckBoxValue							m_RunAnimation;
	GUI::Input::C_Slider<float>							m_AnimationProgress;
	Physics::Primitives::S_AABB							m_AABB;
	bool												m_bInit = false;

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