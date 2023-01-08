#include <GLRendererStdafx.h>

#include <GLRenderer/Buffers/UBO/JointTransformsUBO.h>
#include <GLRenderer/Buffers/UniformBuffersManager.h>
#include <GLRenderer/Commands/HACK/DrawStaticMesh.h>
#include <GLRenderer/Commands/HACK/LambdaCommand.h>
#include <GLRenderer/Components/SkeletalMesh.h>
#include <GLRenderer/Mesh/StaticMeshResource.h>
#include <GLRenderer/Shaders/ShaderManager.h>
#include <GLRenderer/Shaders/ShaderProgram.h>
#include <GLRenderer/Textures/Texture.h>
#include <GLRenderer/Textures/TextureUnitManager.h>

#include <Renderer/Animation/ColladaLoading/ColladaLoader.h>
#include <Renderer/Animation/Skeleton.h>
#include <Renderer/IDevice.h>
#include <Renderer/IRenderer.h>
#include <Renderer/Mesh/Scene.h>

#include <Core/Application.h>
#include <Core/Resources/ResourceManager.h>

#include <pugixml.hpp>

#include <imgui.h>

namespace GLEngine::GLRenderer::Components {

//=================================================================================
C_SkeletalMesh::C_SkeletalMesh(std::shared_ptr<Entity::I_Entity> owner, const std::filesystem::path& meshFile, const std::filesystem::path& meshFolder)
	: Renderer::I_RenderableComponent(owner)
	, m_RenderMesh(true, "Render mesh")
	, m_RunAnimation(true, "Run animation")
	, m_ColorMapGUI(m_ColorMap)
	, m_Animation(0)
	, m_AnimationProgress(.0f, .0f, 1.f, "Animation progress")
	, m_TransformationUBO(nullptr)
	, m_ColorMap(nullptr)
	, m_triangles(0)
{
	m_ColorMapGUI.SetOnTextureCleanCB([&]() {
		m_ColorMapRes = {};
		if (m_ColorMap)
		{
			auto& device = Core::C_Application::Get().GetActiveRenderer().GetDevice();
			device.DestroyTexture(*m_ColorMap.get());
			m_ColorMap = nullptr;
		}
	});
	Renderer::C_ColladaLoader sl;

	std::string textureName;

	Renderer::MeshData::Mesh		  mesh;
	Renderer::MeshData::SkeletonData animData;

	if (!sl.addModelFromDAEFileToScene(meshFolder, meshFile, mesh, textureName, m_Skeleton, m_Animation, animData, m_ModelMatrix))
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "Unable to load model {}/{}", meshFolder, meshFile);
		return;
	}

	SetColorMapPath(meshFolder / textureName);

	// setup joint transforms
	auto& UBOMan		= Buffers::C_UniformBuffersManager::Instance();
	m_TransformationUBO = UBOMan.CreateUniformBuffer<Buffers::UBO::C_JointTramsformsUBO>("jointTransforms", m_Skeleton.GetNumBones());

	// m_TransformationUBO->SetTransforms(m_Animation.GetTransform(0.f));

	// setup VAO
	static_assert(sizeof(glm::vec3) == sizeof(GLfloat) * 3, "Platform doesn't support this directly.");

	m_triangles = mesh.vertices.size();
	m_AABB		= mesh.bbox;

	m_VAO.bind();
	m_VAO.SetBuffer<0, GL_ARRAY_BUFFER>(mesh.vertices);
	m_VAO.SetBuffer<1, GL_ARRAY_BUFFER>(mesh.normals);
	m_VAO.SetBuffer<2, GL_ARRAY_BUFFER>(mesh.texcoords);
	m_VAO.SetBuffer<3, GL_ARRAY_BUFFER>(animData.jointIndices);
	m_VAO.SetBuffer<4, GL_ARRAY_BUFFER>(animData.weights);


	m_VAO.EnableArray<0>();
	m_VAO.EnableArray<1>();
	m_VAO.EnableArray<2>();
	m_VAO.EnableArray<3>();
	m_VAO.EnableArray<4>();

	m_VAO.unbind();

	m_AABB = mesh.bbox;
}

//=================================================================================
C_SkeletalMesh::~C_SkeletalMesh()
{
	if (m_ColorMap)
	{
		auto& device = Core::C_Application::Get().GetActiveRenderer().GetDevice();
		device.DestroyTexture(*m_ColorMap.get());
		m_ColorMap = nullptr;
	}
}

//=================================================================================
void C_SkeletalMesh::DebugDrawGUI()
{
	const static auto zeroVec = glm::vec4(0.f, 0.f, .0f, 1.f);
	m_RenderMesh.Draw();
	m_RunAnimation.Draw();
	m_AnimationProgress.Draw();
	m_ColorMapGUI.Draw();

	std::function<void(const Renderer::S_Joint&)> DrawJointGUI;
	DrawJointGUI = [&DrawJointGUI](const Renderer::S_Joint& joint) {
		if (::ImGui::CollapsingHeader(joint.m_Name.c_str()))
		{
			const auto& pos = (joint.GetAnimatedTransform()) * zeroVec;
			::ImGui::Text("Original pos: [%f, %f, %f]", pos.x, pos.y, pos.z);
			for (const auto& child : joint.m_Children)
			{
				DrawJointGUI(child);
			}
		}
	};

	DrawJointGUI(*(m_Skeleton.m_Root.get()));
}

//=================================================================================
std::string_view C_SkeletalMesh::GetDebugComponentName() const
{
	return "Skeleton";
}

//=================================================================================
bool C_SkeletalMesh::HasDebugDrawGUI() const
{
	return true;
}

//=================================================================================
void C_SkeletalMesh::PerformDraw() const
{
	if (!m_RenderMesh)
	{
		return;
	}
	auto& tm = Textures::C_TextureUnitManger::Instance();
	if (m_ColorMap)
	{
		tm.BindTextureToUnit(*std::static_pointer_cast<Textures::C_Texture>(m_ColorMap), 0);
	}

	auto& shmgr	 = Shaders::C_ShaderManager::Instance();
	auto  shader = shmgr.GetProgram("animation");
	shmgr.ActivateShader(shader);

	Core::C_Application::Get().GetActiveRenderer().AddCommand(std::make_unique<Commands::HACK::C_LambdaCommand>(
		[&, shader]() {
			shader->SetUniform("modelMatrix", GetComponentModelMatrix());

			m_VAO.bind();

			const auto& pose	   = m_Animation.GetPose(m_AnimationProgress.GetValue());
			auto		transofrms = pose.GetLocalSpaceTransofrms();

			m_Skeleton.ApplyPoseToBones(transofrms);

			shader->SetUniform("transforms", transofrms);

			// this wont work with my current UBO manager so that means! @todo!
			// m_TransformationUBO->UploadData();
			// m_TransformationUBO->Activate(true);

			glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(m_triangles));
			m_VAO.unbind();
		},
		"SkeletalMesh - Draw"));
}

//=================================================================================
void C_SkeletalMesh::Update()
{
	if (m_ColorMap == nullptr && m_ColorMapRes.IsReady())
	{
		auto&								  device   = Core::C_Application::Get().GetActiveRenderer().GetDevice();
		auto&								  resource = m_ColorMapRes.GetResource();
		const Renderer::I_TextureViewStorage& storage  = resource.GetStorage();
		m_ColorMap
			= device.CreateTextureHandle(Renderer::TextureDescriptor{.name	 = resource.GetFilepath().generic_string(),
																	 .width	 = storage.GetDimensions().x,
																	 .height = storage.GetDimensions().y,
																	 .type	 = Renderer::E_TextureType::TEXTURE_2D,
																	 .format = Renderer::GetClosestFormat(storage.GetChannels(), !Renderer::IsIntegral(storage.GetStorageType())),
																	 .m_bStreamable = false});
		device.AllocateTexture(*m_ColorMap.get());
		m_ColorMap->SetTexData2D(0, (&storage));

		m_ColorMapGUI = GUI::C_Texture(m_ColorMap);
	}

	const auto& pose = m_Animation.GetPose(m_AnimationProgress.GetValue());
	C_DebugDraw::Instance().DrawSkeleton(glm::vec3(1.0f, .0f, .0f), m_Skeleton);
	C_DebugDraw::Instance().DrawPose(m_Skeleton, pose, GetComponentModelMatrix());
	if (m_RunAnimation)
	{
		m_AnimationProgress += .01f;
		if (m_AnimationProgress.GetValue() > 1.f)
		{
			m_AnimationProgress = 0.f;
		}
	}
}

//=================================================================================
Physics::Primitives::S_AABB C_SkeletalMesh::GetAABB() const
{
	return m_AABB;
}

//=================================================================================
void C_SkeletalMesh::SetColorMapPath(const std::filesystem::path& path)
{
	auto& rm	  = Core::C_ResourceManager::Instance();
	m_ColorMapRes = rm.LoadResource<Renderer::TextureResource>(path);
}

//=================================================================================
const std::filesystem::path& C_SkeletalMesh::GetColorMapPath() const
{
	if (m_ColorMapRes)
		return m_ColorMapRes.GetResource().GetFilepath();
	return {};
}

//=================================================================================
// C_SkeletalMeshBuilder
//=================================================================================
std::shared_ptr<Entity::I_Component> C_SkeletalMeshBuilder::Build(const pugi::xml_node& node, std::shared_ptr<Entity::I_Entity> owner)
{
	if (const auto fileAttr = node.attribute("file"))
	{
		const auto file = fileAttr.as_string();

		return std::make_shared<C_SkeletalMesh>(owner, file);
	}
	else
	{
		CORE_LOG(E_Level::Error, E_Context::Entity, "Given element <SkeletalMesh> doesn't specify file to load");
	}

	return nullptr;
}

} // namespace GLEngine::GLRenderer::Components
