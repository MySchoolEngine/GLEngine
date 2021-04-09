#include <GLRendererStdafx.h>

#include <GLRenderer/Buffers/UBO/ModelData.h>
#include <GLRenderer/Buffers/UniformBuffersManager.h>
#include <GLRenderer/Commands/HACK/DrawStaticMesh.h>
#include <GLRenderer/Commands/HACK/LambdaCommand.h>
#include <GLRenderer/Components/StaticMesh.h>
#include <GLRenderer/Mesh/StaticMeshResource.h>
#include <GLRenderer/Shaders/ShaderManager.h>
#include <GLRenderer/Shaders/ShaderProgram.h>
#include <GLRenderer/Textures/Texture.h>
#include <GLRenderer/Textures/TextureManager.h>

#include <Renderer/IRenderer.h>
#include <Renderer/Materials/Material.h>
#include <Renderer/Materials/MaterialManager.h>
#include <Renderer/Mesh/Loading/SceneLoader.h>
#include <Renderer/Mesh/Scene.h>

#include <Core/Application.h>

#include <Utils/Parsing/MaterialParser.h>

#include <pugixml.hpp>

#include <imgui.h>

namespace GLEngine::GLRenderer::Components {

//=================================================================================
C_StaticMesh::C_StaticMesh(std::string meshFile, std::string_view shader, std::shared_ptr<Entity::I_Entity> owner)
	: Renderer::I_RenderableComponent(owner)
	, m_meshFile(meshFile)
	, m_Mesh(nullptr)
	, m_Material(nullptr)
{
	// @todo lazy init
	auto sl = std::make_unique<Renderer::Mesh::SceneLoader>();

	auto	  scene		  = std::make_shared<Renderer::MeshData::Scene>();
	glm::mat4 modelMatrix = glm::mat4(1.0f);

	if (!sl->addModelFromFileToScene("Models", m_meshFile.c_str(), scene, modelMatrix))
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "Unable to load model {}", m_meshFile);
		return;
	}

	m_Transformation.SetMatrix(modelMatrix);

	// TODO this is unsafe way to init model
	m_Mesh		= std::make_shared<Mesh::C_StaticMeshResource>(scene->meshes[0]);
	m_AABB		= scene->meshes[0].bbox;
	auto& shmgr = Shaders::C_ShaderManager::Instance();
	m_Shader	= shmgr.GetProgram(shader.data());

	const auto	materialIdx = scene->meshes[0].materialIndex;
	const auto& material	= scene->materials[materialIdx];

	SetMaterial(material);
}

//=================================================================================
C_StaticMesh::C_StaticMesh(const Renderer::MeshData::Mesh& mesh, std::string_view shader, std::shared_ptr<Entity::I_Entity> owner, const Renderer::MeshData::Material* material)
	: Renderer::I_RenderableComponent(owner)
	, m_Material(nullptr)
{
	m_Mesh = std::make_shared<Mesh::C_StaticMeshResource>(mesh);
	m_AABB = mesh.bbox;

	auto& shmgr = Shaders::C_ShaderManager::Instance();
	m_Shader	= shmgr.GetProgram(shader.data());

	if (material)
		SetMaterial(*material);
}

//=================================================================================
void C_StaticMesh::PerformDraw() const
{
	if (!m_Mesh || !m_Shader)
	{
		// CORE_LOG(E_Level::Error, E_Context::Render, "Static mesh uncomplete");
		return;
	}
	auto& renderer = Core::C_Application::Get().GetActiveRenderer();


	auto& shmgr = Shaders::C_ShaderManager::Instance();
	auto& tmgr	= Textures::C_TextureManager::Instance();
	if (renderer.GetCurrentPassType() == Renderer::E_PassType::ShadowPass && m_ShadowPassShader)
	{
		shmgr.ActivateShader(m_ShadowPassShader);
	}
	else
	{
		shmgr.ActivateShader(m_Shader);
	}

	renderer.AddCommand(std::move(std::make_unique<Commands::HACK::C_LambdaCommand>(
		[&, matIndex = m_Material ? m_Material->GetMaterialIndex() : 0]() {
			auto modelData = Buffers::C_UniformBuffersManager::Instance().GetBufferByName("modelData");
			if (auto modelDataUbo = std::dynamic_pointer_cast<Buffers::UBO::C_ModelData>(modelData))
			{
				modelDataUbo->SetModelMatrix(GetComponentModelMatrix());
				modelDataUbo->SetMaterialIndex(matIndex);
				modelDataUbo->UploadData();
			}
		},
		"Per object data upload")));

	renderer.AddCommand(std::move(std::make_unique<Commands::HACK::C_DrawStaticMesh>(m_Mesh)));
}

//=================================================================================
void C_StaticMesh::DebugDrawGUI()
{
	m_Color.Draw();
	if (!m_RoughnessMap)
	{
		m_Roughness.Draw();
	}
	else
	{
		ImGui::Image((void*)(intptr_t)(m_RoughnessMap->GetTexture()), ImVec2(128, 128));
	}
	if (m_ColorMap)
	{
		ImGui::Image((void*)(intptr_t)(m_ColorMap->GetTexture()), ImVec2(128, 128));
	}
	if (m_NormalMap)
	{
		ImGui::Image((void*)(intptr_t)(m_NormalMap->GetTexture()), ImVec2(128, 128));
	}
}

//=================================================================================
void C_StaticMesh::SetMaterial(const Renderer::MeshData::Material& material)
{
	auto& materialManager = Renderer::C_MaterialManager::Instance();
	m_Material			  = materialManager.GetMaterial(material.m_Name);
	if (!m_Material)
	{
		m_Material = materialManager.RegisterMaterial(Renderer::C_Material(material));
	}
}

//=================================================================================
std::string_view C_StaticMesh::GetDebugComponentName() const
{
	return "Static mesh";
}

//=================================================================================
bool C_StaticMesh::HasDebugDrawGUI() const
{
	return true;
}

//=================================================================================
GLEngine::Physics::Primitives::S_AABB C_StaticMesh::GetAABB() const
{
	return m_AABB;
}

//=================================================================================
void C_StaticMesh::SetColorMap(std::shared_ptr<Textures::C_Texture>& texture)
{
	m_ColorMap = texture;
	m_ColorMap->CreateHandle();

	m_Material->SetColorMap(static_cast<void*>(&m_ColorMap));
}

//=================================================================================
void C_StaticMesh::SetNormalMap(std::shared_ptr<Textures::C_Texture>& texture)
{
	m_NormalMap = texture;
	texture->CreateHandle();
	m_Material->SetNormalMap(static_cast<void*>(&m_NormalMap));
}

//=================================================================================
void C_StaticMesh::SetRoughnessMap(const std::shared_ptr<Textures::C_Texture>& texture)
{
	m_RoughnessMap = texture;
	m_Roughness	   = 1.0f;
	texture->CreateHandle();
	m_Material->SetRoughnessMap(static_cast<void*>(&m_RoughnessMap));
}

//=================================================================================
std::shared_ptr<Entity::I_Component> C_StaticMeshBuilder::Build(const pugi::xml_node& node, std::shared_ptr<Entity::I_Entity> owner)
{
	const auto materialData = Utils::Parsing::C_MaterialParser::ParseMaterialData(node);

	auto staticMesh = std::make_shared<C_StaticMesh>(node.attribute("filePath").value(), materialData.m_MaterialName, owner);

	if (auto shadowPassAttr = node.attribute("shadowPassShader"))
	{
		auto& shmgr			   = Shaders::C_ShaderManager::Instance();
		auto  shadowPassShader = shmgr.GetProgram(shadowPassAttr.as_string());
		if (shadowPassShader)
		{
			staticMesh->m_ShadowPassShader = shadowPassShader;
		}
	}

	staticMesh->SetColor(materialData.m_Color);
	staticMesh->m_Roughness = materialData.m_Roughness;


	auto& tmgr = Textures::C_TextureManager::Instance();

	if (!materialData.m_RoughtnessMap.empty())
	{
		auto roughnessMap = tmgr.GetTexture(materialData.m_RoughtnessMap);
		if (roughnessMap)
		{
			roughnessMap->StartGroupOp();
			roughnessMap->SetWrap(Renderer::E_WrapFunction::Repeat, Renderer::E_WrapFunction::Repeat);
			roughnessMap->SetFilter(Renderer::E_TextureFilter::LinearMipMapLinear, Renderer::E_TextureFilter::Linear);
			roughnessMap->GenerateMipMaps();

			roughnessMap->EndGroupOp();

			staticMesh->SetRoughnessMap(roughnessMap);
		}
	}

	if (!materialData.m_ColorMap.empty())
	{
		auto colorMapTexture = tmgr.GetTexture(materialData.m_ColorMap);
		if (colorMapTexture)
		{
			colorMapTexture->StartGroupOp();
			colorMapTexture->SetWrap(Renderer::E_WrapFunction::Repeat, Renderer::E_WrapFunction::Repeat);
			colorMapTexture->SetFilter(Renderer::E_TextureFilter::LinearMipMapLinear, Renderer::E_TextureFilter::Linear);
			colorMapTexture->GenerateMipMaps();

			colorMapTexture->EndGroupOp();

			staticMesh->SetColorMap(colorMapTexture);
		}
	}

	if (!materialData.m_NormalMap.empty())
	{
		auto normalMap = tmgr.GetTexture(materialData.m_NormalMap);
		if (normalMap)
		{
			normalMap->StartGroupOp();
			normalMap->SetWrap(Renderer::E_WrapFunction::Repeat, Renderer::E_WrapFunction::Repeat);
			normalMap->SetFilter(Renderer::E_TextureFilter::LinearMipMapLinear, Renderer::E_TextureFilter::Linear);
			normalMap->GenerateMipMaps();

			normalMap->EndGroupOp();

			staticMesh->SetNormalMap(normalMap);
		}
	}

	return staticMesh;
}

} // namespace GLEngine::GLRenderer::Components