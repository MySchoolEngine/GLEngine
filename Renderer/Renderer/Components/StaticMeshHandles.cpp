#include <RendererStdafx.h>

#include <Renderer/Components/StaticMeshHandles.h>
#include <Renderer/IRenderer.h>
#include <Renderer/Materials/Material.h>
#include <Renderer/Materials/MaterialManager.h>
#include <Renderer/Mesh/Loading/ModelLoader.h>
#include <Renderer/Resources/ResourceManager.h>

#include <Core/Application.h>
#include <Core/Resources/LoadingQuery.h>
#include <Core/Resources/ResourceManager.h>

#include <Utils/Reflection/Metadata.h>

#include <imgui.h>
#include <rttr/registration>

RTTR_REGISTRATION
{
	using namespace GLEngine::Renderer;
	using namespace Utils::Reflection;

	rttr::registration::class_<C_StaticMeshHandles>("C_StaticMeshHandles")
		.constructor<>()(rttr::policy::ctor::as_std_shared_ptr)
		.property("MeshFile", &C_StaticMeshHandles::GetMeshFile, &C_StaticMeshHandles::SetMeshFile)(RegisterMetamember<SerializationCls::MandatoryProperty>(true))
		.property("Material", &C_StaticMeshHandles::m_Material)
		//.property("Shader", &C_StaticMesh::GetShader, &C_StaticMesh::SetShader)
		;
	rttr::type::register_wrapper_converter_for_base_classes<std::shared_ptr<C_StaticMeshHandles>>();
	rttr::type::register_converter_func([](std::shared_ptr<C_StaticMeshHandles> ptr, bool& ok) -> std::shared_ptr<GLEngine::Entity::I_Component> {
		ok = true;
		return std::static_pointer_cast<GLEngine::Entity::I_Component>(ptr);
	});
}

namespace GLEngine::Renderer {

//=================================================================================
C_StaticMeshHandles::C_StaticMeshHandles()
	: I_RenderableComponent(nullptr)
	, m_Material(nullptr)
{
	SetMeshFile(R"(Models/sphere.obj)");
}

//=================================================================================
bool C_StaticMeshHandles::HasDebugDrawGUI() const
{
	return true;
}

//=================================================================================
std::string_view C_StaticMeshHandles::GetDebugComponentName() const
{
	return "Static component (handles)";
}

//=================================================================================
void C_StaticMeshHandles::OnEvent(Core::I_Event& event)
{
}

//=================================================================================
void C_StaticMeshHandles::PerformDraw() const
{
	if (!m_MeshResource)
		return;
}

//=================================================================================
Physics::Primitives::S_AABB C_StaticMeshHandles::GetAABB() const
{
	if (m_MeshResource)
		return m_MeshResource.GetResource().GetScene().bbox;
	return {};
}

//=================================================================================
void C_StaticMeshHandles::Update()
{
	if (m_MeshResource.IsReady() && !m_Material)
	{
		m_Material = Renderer::C_MaterialManager::Instance().RegisterMaterial(Renderer::C_Material(m_MeshResource.GetResource().GetScene().materials[0]));
	}
	if (m_Pipeline.IsValid() == false)
	{
		GLE_TODO("6-11-2024", "RohacekD", "Can we validate the definition with shaders?");
		m_Pipeline = Core::C_Application::Get().GetActiveRenderer().GetRM().createPipeline(PipelineDescriptor{
		.primitiveType = Renderer::E_RenderPrimitives::TriangleList,
		.bindingCount = 5,
		.vertexInput   = {{
							  .binding = 0,
							  .type	   = Renderer::T_TypeShaderDataType_v<glm::vec3>
						  },
						  {
							  .binding = 1,
							  .type	   = Renderer::T_TypeShaderDataType_v<glm::vec3>
						  },
						  {
							  .binding = 2,
							  .type	   = Renderer::T_TypeShaderDataType_v<glm::vec2>
						  },
						  {
							  .binding = 3,
							  .type	   = Renderer::T_TypeShaderDataType_v<glm::vec3>
						  },
						  {
							  .binding = 4,
							  .type	   = Renderer::T_TypeShaderDataType_v<glm::vec3>
						  },
			},
		.shader =  "handles",
		// .colorAttachementFormat = GetTextureFormat(m_SwapChainImageFormat), no need for OpenGL and will be solved for Vulkan
	});
	}
	if (m_Meshes.empty() == true && m_MeshResource.IsReady())
	{
		auto& scene = m_MeshResource.GetResource().GetScene();
		m_Meshes.reserve(scene.meshes.size());
		for (auto& mesh : scene.meshes)
		{
			auto& meshContainer			  = m_Meshes.emplace_back();
			meshContainer.m_NumPrimitives = static_cast<uint32_t>(mesh.vertices.size());
			// load buffer

			I_Renderer&		renderer = Core::C_Application::Get().GetActiveRenderer();
			ResouceManager& rm		 = renderer.GetRM();

			const auto positionsSize		= static_cast<uint32_t>(sizeof(mesh.vertices[0]) * mesh.vertices.size());
			meshContainer.m_PositionsHandle = rm.createBuffer(BufferDescriptor{
				.size  = positionsSize,
				.type  = E_BufferType::Vertex,
				.usage = E_ResourceUsage::Immutable,
			});
			renderer.SetBufferData(meshContainer.m_PositionsHandle, positionsSize, mesh.vertices.data());

			const auto normalsSize		  = static_cast<uint32_t>(sizeof(mesh.normals[0]) * mesh.normals.size());
			meshContainer.m_NormalsHandle = rm.createBuffer(BufferDescriptor{
				.size  = normalsSize,
				.type  = E_BufferType::Vertex,
				.usage = E_ResourceUsage::Immutable,
			});
			renderer.SetBufferData(meshContainer.m_NormalsHandle, normalsSize, mesh.normals.data());

			const auto texCoordSize			= static_cast<uint32_t>(sizeof(mesh.texcoords[0]) * mesh.texcoords.size());
			meshContainer.m_TexCoordsHandle = rm.createBuffer(BufferDescriptor{
				.size  = texCoordSize,
				.type  = E_BufferType::Vertex,
				.usage = E_ResourceUsage::Immutable,
			});
			renderer.SetBufferData(meshContainer.m_TexCoordsHandle, texCoordSize, mesh.texcoords.data());

			const auto tangentSize		  = static_cast<uint32_t>(sizeof(mesh.tangent[0]) * mesh.tangent.size());
			meshContainer.m_TangentHandle = rm.createBuffer(BufferDescriptor{
				.size  = tangentSize,
				.type  = E_BufferType::Vertex,
				.usage = E_ResourceUsage::Immutable,
			});
			renderer.SetBufferData(meshContainer.m_TangentHandle, tangentSize, mesh.tangent.data());

			const auto bitangentSize		= static_cast<uint32_t>(sizeof(mesh.bitangent[0]) * mesh.bitangent.size());
			meshContainer.m_BitangentHandle = rm.createBuffer(BufferDescriptor{
				.size  = bitangentSize,
				.type  = E_BufferType::Vertex,
				.usage = E_ResourceUsage::Immutable,
			});
			renderer.SetBufferData(meshContainer.m_BitangentHandle, bitangentSize, mesh.bitangent.data());
		}
	}
}

//=================================================================================
void C_StaticMeshHandles::Render(Renderer3D& renderer) const
{
	if (m_MeshResource.IsReady() && m_Material)
	{
		for (auto& meshContainer : m_Meshes)
		{
			renderer.Draw(RenderCall3D{
				.ModelMatrix   = GetComponentModelMatrix(),
				.NumPrimities  = meshContainer.m_NumPrimitives,
				.MaterialIndex = m_Material->GetMaterialIndex(),
				.Buffers
				= {meshContainer.m_PositionsHandle, meshContainer.m_NormalsHandle, meshContainer.m_TexCoordsHandle, meshContainer.m_TangentHandle, meshContainer.m_BitangentHandle},
				.PipelineHandle = m_Pipeline,
			});
		}
	}
}

//=================================================================================
void C_StaticMeshHandles::SetMeshFile(const std::filesystem::path meshfile)
{
	auto& rm   = Core::C_ResourceManager::Instance();
	auto  path = meshfile;
	if (meshfile.generic_string().find("Models") == std::string::npos)
		path = std::filesystem::path("Models") / meshfile;
	m_MeshResource = rm.LoadResource<Renderer::MeshResource>(path);
}

//=================================================================================
std::filesystem::path C_StaticMeshHandles::GetMeshFile() const
{
	return m_MeshResource.GetResource().GetFilePath();
}

//=================================================================================
void C_StaticMeshHandles::DebugDrawGUI()
{
	if (::ImGui::CollapsingHeader("Material"))
	{
		if (m_Material)
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
}

} // namespace GLEngine::Renderer