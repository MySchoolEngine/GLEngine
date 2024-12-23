#include <GLRendererStdafx.h>

#include <GLRenderer/Buffers/UBO/ModelData.h>
#include <GLRenderer/Buffers/UniformBuffersManager.h>
#include <GLRenderer/Commands/HACK/DrawStaticMesh.h>
#include <GLRenderer/Commands/HACK/LambdaCommand.h>
#include <GLRenderer/Components/StaticMesh.h>
#include <GLRenderer/Mesh/StaticMeshResource.h>
#include <GLRenderer/Shaders/ShaderManager.h>
#include <GLRenderer/Shaders/ShaderProgram.h>

#include <Renderer/IRenderer.h>
#include <Renderer/Materials/Material.h>
#include <Renderer/Materials/MaterialManager.h>
#include <Renderer/Mesh/Scene.h>

#include <GUI/ReflectionGUI.h>

#include <Core/Application.h>
#include <Core/Resources/ResourceManager.h>

#include <Utils/Reflection/Metadata.h>
#include <Utils/Serialization/SerializationUtils.h>

#include <imgui.h>
#include <rttr/registration>

// clang-format off
RTTR_REGISTRATION
{
	using namespace GLEngine::GLRenderer::Components;
	using namespace Utils::Reflection;

	rttr::registration::class_<C_StaticMesh>("C_StaticMesh")
		.constructor<std::string, std::string_view, std::shared_ptr<GLEngine::Entity::I_Entity>>()
		.constructor<>()(rttr::policy::ctor::as_std_shared_ptr)
		.property("MeshFile", &C_StaticMesh::GetMeshFile, &C_StaticMesh::SetMeshFile)
		(
			RegisterMetamember<SerializationCls::MandatoryProperty>(true)
		)
		.property("Material", &C_StaticMesh::m_Material)
		.property("ModelRes", &C_StaticMesh::m_MeshResource)(
			rttr::policy::prop::as_reference_wrapper,
			RegisterMetaclass<MetaGUI::MeshResource>(),
			RegisterMetamember<UI::MeshResource::Name>("Model"),
			REGISTER_DEFAULT_VALUE(GLEngine::Core::ResourceHandle<GLEngine::Renderer::MeshResource>()))
		.property("Shader", &C_StaticMesh::GetShader, &C_StaticMesh::SetShader)
		.property("ShadowPassShader", &C_StaticMesh::GetShadowShader, &C_StaticMesh::SetShadowShader);

	rttr::type::register_wrapper_converter_for_base_classes<std::shared_ptr<C_StaticMesh>>();
	rttr::type::register_converter_func([](std::shared_ptr<C_StaticMesh> ptr, bool& ok) -> std::shared_ptr<GLEngine::Entity::I_Component> {
		ok = true;
		return std::static_pointer_cast<GLEngine::Entity::I_Component>(ptr);
	});
}
// clang-format on

namespace GLEngine::GLRenderer::Components {

//=================================================================================
C_StaticMesh::C_StaticMesh(std::string meshFile, const std::string_view shader, const std::shared_ptr<Entity::I_Entity>& owner)
	: Renderer::I_RenderableComponent(owner)
	, m_Material(nullptr)
{
	SetMeshFile(meshFile);
	SetShader(shader.data());
}

//=================================================================================
C_StaticMesh::C_StaticMesh(const Core::ResourceHandle<Renderer::MeshResource>& meshHandle,
						   const std::string_view							   shader,
						   const std::shared_ptr<Entity::I_Entity>&			   owner,
						   const Renderer::MeshData::Material*				   material)
	: Renderer::I_RenderableComponent(owner)
	, m_MeshResource(meshHandle)
	, m_Material(nullptr)
{
	SetShader(shader.data());

	if (material)
		SetMaterial(*material);
}

//=================================================================================
C_StaticMesh::C_StaticMesh()
	: Renderer::I_RenderableComponent(nullptr)
	, m_Material(nullptr)
{
	SetShader("basicTracing"); // TODO
	Renderer::MeshData::Material material{glm::vec4{1.0f, 0.0f, 0.f, 0.f}, glm::vec4{1.0f, 0.0f, 0.f, 0.f}, glm::vec4{1.0f, 0.0f, 0.f, 0.f}, .5f, -1, -1, "Default"};
	SetMaterial(material);
}

//=================================================================================
C_StaticMesh::~C_StaticMesh()
{
	auto& materialManager = Renderer::C_MaterialManager::Instance();
	if (m_Material)
		materialManager.UnregisterMaterial(m_Material);
}

//=================================================================================
void C_StaticMesh::PerformDraw() const
{
	if (m_Mesh.empty() || !m_Shader || !m_MeshResource)
	{
		// lazy load, lazy dominik wrong design, should burn in hell
		return;
	}
	auto& renderer = Core::C_Application::Get().GetActiveRenderer();

	if (m_MeshResource.GetResource().GetScene().meshes[0].skeleton.bones.empty() == false)
	{
		constexpr glm::vec4 zero(0, 0, 0, 1);
		auto&				dd = C_DebugDraw::Instance();
		for (const auto& bone : m_MeshResource.GetResource().GetScene().meshes[0].skeleton.bones)
		{
			dd.DrawPoint(glm::transpose(bone.modelSpace) * zero, Colours::white);
		}
	}


	auto& shmgr = Shaders::C_ShaderManager::Instance();
	if (renderer.GetCurrentPassType() == Renderer::E_PassType::ShadowPass && m_ShadowPassShader)
	{
		shmgr.ActivateShader(m_ShadowPassShader);
	}
	else
	{
		shmgr.ActivateShader(m_Shader);
	}

	renderer.AddCommand(std::make_unique<Commands::HACK::C_LambdaCommand>(
		[&, matIndex = m_Material ? m_Material->GetMaterialIndex() : 0]() {
			auto modelData = Buffers::C_UniformBuffersManager::Instance().GetBufferByName("modelData");
			if (auto modelDataUbo = std::dynamic_pointer_cast<Buffers::UBO::C_ModelData>(modelData))
			{
				modelDataUbo->SetModelMatrix(GetComponentModelMatrix());
				modelDataUbo->SetMaterialIndex(matIndex);
				modelDataUbo->UploadData();
			}
		},
		"Per object data upload"));

	for (const auto& mesh : m_Mesh)
		renderer.AddCommand(std::make_unique<Commands::HACK::C_DrawStaticMesh>(mesh));
}

//=================================================================================
void C_StaticMesh::DebugDrawGUI()
{
	if (::ImGui::CollapsingHeader("Material"))
	{
		m_Material->DrawGUI();
	}
	if (m_MeshResource.IsLoading())
	{
		::ImGui::Text("Still loading");
	}
	else if (m_MeshResource.IsFailed())
	{
		::ImGui::TextColored(ImVec4(1, 0, 0, 1), "Failed");
	}
	rttr::instance obj(*this);
	if (GUI::DrawAllPropertyGUI(obj).empty() == false)
	{
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
void C_StaticMesh::SetMaterial(const std::shared_ptr<Renderer::C_Material>& material)
{
	auto& materialManager = Renderer::C_MaterialManager::Instance();
	if (m_Material)
		materialManager.UnregisterMaterial(m_Material);
	m_Material = material;
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
Physics::Primitives::S_AABB C_StaticMesh::GetAABB() const
{
	return m_AABB;
}

//=================================================================================
void C_StaticMesh::Update()
{
	if (m_MeshResource && m_Mesh.empty())
	{
		const auto& scene = m_MeshResource.GetResource().GetScene();
		for (const auto& mesh : scene.meshes)
		{
			m_Mesh.emplace_back(std::make_shared<Mesh::C_StaticMeshResource>(mesh));
			m_AABB.Add(mesh.bbox);
		}

		// only 1 material right now
		if (!m_Material || m_Material->GetName() == "Default")
		{
			const auto materialIdx = scene.meshes[0].materialIndex;
			// would force all materials to be white
			auto& material = scene.materials[materialIdx];
			SetMaterial(material);
			if (material.textureIndex != -1)
			{
				m_Material->SetColorMapPath(m_MeshResource.GetResource().GetTextureNames()[material.textureIndex]);
			}
		}
	}
}

//=================================================================================
void C_StaticMesh::SetShader(const std::string shader)
{
	auto& shmgr = Shaders::C_ShaderManager::Instance();
	m_Shader	= shmgr.GetProgram(shader);
}

//=================================================================================
std::string C_StaticMesh::GetShader() const
{
	if (!m_Shader)
		return "";
	else
		return m_Shader->GetName();
}

//=================================================================================
void C_StaticMesh::SetShadowShader(const std::string shader)
{
	if (shader.empty())
	{
		m_ShadowPassShader = nullptr;
		return;
	}
	auto& shmgr		   = Shaders::C_ShaderManager::Instance();
	m_ShadowPassShader = shmgr.GetProgram(shader);
}

//=================================================================================
std::string C_StaticMesh::GetShadowShader() const
{
	if (!m_ShadowPassShader)
		return "";
	else
		return m_ShadowPassShader->GetName();
}

//=================================================================================
void C_StaticMesh::SetMeshFile(const std::filesystem::path meshfile)
{
	auto& rm   = Core::C_ResourceManager::Instance();
	auto  path = meshfile;
	if (meshfile.generic_string().find("Models") == std::string::npos)
		path = std::filesystem::path("Models") / meshfile;
	m_MeshResource = rm.LoadResource<Renderer::MeshResource>(path);
}

//=================================================================================
std::filesystem::path C_StaticMesh::GetMeshFile() const
{
	if (m_MeshResource.IsReady())
		return m_MeshResource.GetFilepath();
	return {};
}

} // namespace GLEngine::GLRenderer::Components