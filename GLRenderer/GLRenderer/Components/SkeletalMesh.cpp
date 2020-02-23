#include <GLRendererStdAfx.h>

#include <GLRenderer/Components/SkeletalMesh.h>

#include <GLRenderer/Buffers/UBO/JointTransformsUBO.h>
#include <GLRenderer/Buffers/UniformBuffersManager.h>

#include <GLRenderer/Shaders/ShaderManager.h>
#include <GLRenderer/Shaders/ShaderProgram.h>

#include <Renderer/Animation/ColladaLoading/ColladaLoader.h>
#include <Renderer/Mesh/Scene.h>

#include <GLRenderer/Textures/TextureLoader.h>
#include <GLRenderer/Textures/TextureUnitManager.h>

#include <GLRenderer/Commands/HACK/DrawStaticMesh.h>
#include <GLRenderer/Commands/HACK/LambdaCommand.h>

#include <Renderer/IRenderer.h>

#include <Utils/HighResolutionTimer.h>

#include <Core/Application.h>


#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

#include <GLRenderer/Debug.h>


#include <pugixml.hpp>

namespace GLEngine::GLRenderer::Components {

//=================================================================================
void C_SkeletalMesh::DebugDrawGUI()
{
	m_RenderMesh.Draw();
	m_AnimationProgress.Draw();

	std::function<void(const Renderer::Animation::S_Joint&)> DrawJointGUI;
	DrawJointGUI = [&DrawJointGUI](const Renderer::Animation::S_Joint& joint)
	{
		if (::ImGui::CollapsingHeader(joint.m_Name.c_str())) {
			auto& pos = (joint.GetAnimatedTransform()) * glm::vec4(0.f, 0.f, .0f, 1.f);
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
	if (!m_RenderMesh)
	{
		return;
	}
	auto& tm = Textures::C_TextureUnitManger::Instance();
	tm.BindTextureToUnit(*m_Texture, 0);
	Core::C_Application::Get().GetActiveRenderer()->AddCommand(
		std::move(
			std::make_unique<Commands::HACK::C_LambdaCommand>(
				[&]() {
					auto& shmgr = Shaders::C_ShaderManager::Instance();
					auto shader = shmgr.GetProgram("animation");
					shmgr.ActivateShader(shader);
	
					shader->SetUniform("modelMatrix", m_ModelMatrix * glm::rotate(-glm::half_pi<float>(), glm::vec3(1.f, .0f, .0f)));

					m_VAO.bind();

					const auto& pose = m_Animation.GetPose(m_AnimationProgress.GetValue());
					auto transofrms = pose.GetModelSpaceTransofrms();

					m_Skeleton.ApplyPoseToBones(transofrms);

					shader->SetUniform("transforms", transofrms);

					// this wont work with my current UBO manager so that means! @todo!
					// m_TransformationUBO->UploadData();
					// m_TransformationUBO->Activate(true);
	
					glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(m_triangles));
					m_VAO.unbind();
				}
				)
		)
	);
}

//=================================================================================
void C_SkeletalMesh::Update()
{
	C_DebugDraw::Instance().DrawSkeleton(glm::vec3(1.0f, .0f, .0f), m_Skeleton);
	m_AnimationProgress += .01f;
	if (m_AnimationProgress.GetValue() > 1.f)
	{
		m_AnimationProgress = 0.f;
	}
}

//=================================================================================
C_SkeletalMesh::C_SkeletalMesh(std::shared_ptr<Entity::I_Entity> owner, std::string meshFile, std::string meshFolder)
	: Renderer::I_RenderableComponent(owner)
	, m_RenderMesh(true, "Render mesh")
	, m_Animation(0)
	, m_AnimationProgress(.0f, .0f, 1.f, "Animation progress")
	, m_TransformationUBO(nullptr)
{
	Renderer::Animation::C_ColladaLoader sl;

	auto scene = std::make_shared<Renderer::MeshData::Scene>();

	std::string textureName;

	Utils::HighResolutionTimer timer;
	timer.reset();

	Renderer::MeshData::Mesh mesh;
	Renderer::MeshData::AnimationData animData;


	if (!sl.addModelFromDAEFileToScene(meshFolder.data(), meshFile.data(), mesh, textureName, m_Skeleton, m_Animation, animData, m_ModelMatrix))
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "Unable to load model {}/{}", meshFolder, meshFile);
		return;
	}

	//m_Mesh = std::make_shared<Mesh::C_StaticMeshResource>(mesh);

	CORE_LOG(E_Level::Info, E_Context::Render, "Parsing skeleton file took {}ms", timer.getElapsedTimeFromLastQueryMilliseconds());

	std::string texurePath(meshFolder + "/");

	texurePath.append(textureName);

	const auto escapeSequence = texurePath.find("%20");
	if (escapeSequence != std::string::npos)
	{
		texurePath.replace(escapeSequence, 3, " ");
	}

	Textures::TextureLoader tl;
	Renderer::MeshData::Texture t;
	bool retval = tl.loadTexture(texurePath.c_str(), t);

	if (!retval)
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "Texture '{}' cannot be loaded", textureName);
	}

	m_Texture = std::make_shared<Textures::C_Texture>(textureName);
	m_Texture->StartGroupOp();
	m_Texture->SetTexData2D(0, t);
	m_Texture->SetWrap(E_WrapFunction::Repeat, E_WrapFunction::Repeat);
	m_Texture->SetFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
	m_Texture->GenerateMipMaps();

	m_Texture->EndGroupOp();

	// setup joint transforms
	auto& UBOMan = Buffers::C_UniformBuffersManager::Instance();
	m_TransformationUBO = UBOMan.CreateUniformBuffer<Buffers::UBO::C_JointTramsformsUBO>("jointTransforms", m_Skeleton.GetNumBones());

	//m_TransformationUBO->SetTransforms(m_Animation.GetTransform(0.f));

	// setup VAO
	static_assert(sizeof(glm::vec3) == sizeof(GLfloat) * 3, "Platform doesn't support this directly.");

	m_triangles = mesh.vertices.size();

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

}