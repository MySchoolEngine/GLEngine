#include <GLRendererStdAfx.h>

#include <GLRenderer/Components/SkeletalMesh.h>

#include <GLRenderer/Shaders/ShaderManager.h>
#include <GLRenderer/Shaders/ShaderProgram.h>

#include <GLRenderer/MeshLoading/SceneLoader.h>
#include <GLRenderer/MeshLoading/Scene.h>

#include <GLRenderer/Textures/TextureLoader.h>
#include <GLRenderer/Textures/TextureUnitManager.h>

#include <GLRenderer/Commands/HACK/DrawStaticMesh.h>
#include <GLRenderer/Commands/HACK/LambdaCommand.h>

#include <Renderer/IRenderer.h>

#include <Utils/HighResolutionTimer.h>

#include <Core/Application.h>

namespace GLEngine::GLRenderer::Components {

//=================================================================================
void C_SkeletalMesh::DebugDrawGUI()
{
	m_RenderMesh.Draw();

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
	if (!m_Mesh || !m_RenderMesh)
	{
		return;
	}
	Core::C_Application::Get().GetActiveRenderer()->AddCommand(
		std::move(
			std::make_unique<Commands::HACK::C_LambdaCommand>(
				[&]() {
					auto& shmgr = Shaders::C_ShaderManager::Instance();
					auto shader = shmgr.GetProgram("basic");
					shmgr.ActivateShader(shader);
	
					shader->SetUniform("modelMatrix", glm::mat4(1.0f));
				}
			)
		)
	);
	auto& tm = Textures::C_TextureUnitManger::Instance();
	tm.BindTextureToUnit(*m_Texture, 0);
	Core::C_Application::Get().GetActiveRenderer()->AddCommand(
		std::move(
			std::make_unique<Commands::HACK::C_DrawStaticMesh>(m_Mesh)
		)
	);
}

//=================================================================================
void C_SkeletalMesh::Update()
{
	C_DebugDraw::Instance().DrawSkeleton(glm::vec3(1.0f, .0f, .0f), m_Skeleton);
}

//=================================================================================
C_SkeletalMesh::C_SkeletalMesh(std::string meshFile, std::string meshFolder)
	: m_RenderMesh(true, "Render mesh")
{
	auto sl = std::make_unique<Mesh::SceneLoader>();

	auto scene = std::make_shared<Mesh::Scene>();
	glm::mat4 modelMatrix = glm::mat4(1.0f);

	std::string textureName;

	Utils::HighResolutionTimer timer;
	timer.reset();

	if (!sl->addModelFromDAEFileToScene(meshFolder.data(), meshFile.data(), m_Mesh, textureName, m_Skeleton, m_Animation, modelMatrix))
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "Unable to load model {}/{}", meshFolder, meshFile);
		return;
	}
	CORE_LOG(E_Level::Info, E_Context::Render, "Parsing skeleton file took {}ms", timer.getElapsedTimeFromLastQueryMilliseconds());

	std::string texurePath(meshFolder + "/");

	texurePath.append(textureName);

	const auto escapeSequence = texurePath.find("%20");
	if (escapeSequence != std::string::npos)
	{
		texurePath.replace(escapeSequence, 3, " ");
	}

	Textures::TextureLoader tl;
	Mesh::Texture t;
	bool retval = tl.loadTexture(texurePath.c_str(), t);

	if (!retval)
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "Texture '{}' cannot be loaded", textureName);
	}

	m_Texture = std::make_shared<Textures::C_Texture>(textureName);
	m_Texture->StartGroupOp();
	glTexImage2D(m_Texture->GetTarget(),
		0,
		GL_RGB,
		(GLsizei)t.width,
		(GLsizei)t.height,
		0,
		GL_RGBA,
		T_TypeToGL<typename std::remove_pointer<decltype(t.data)::element_type>::type>::value,
		t.data.get());
	m_Texture->SetDimensions({ t.width, t.height });
	m_Texture->SetWrap(E_WrapFunction::Repeat, E_WrapFunction::Repeat);
	m_Texture->SetFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
	m_Texture->GenerateMipMaps();

	m_Texture->EndGroupOp();
}

}
