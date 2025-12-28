#include <RendererStdafx.h>

#include <Renderer/Components/StaticMeshHandles.h>
#include <Renderer/IRenderer.h>
#include <Renderer/Materials/Material.h>
#include <Renderer/Materials/MaterialManager.h>
#include <Renderer/Mesh/Loading/ModelLoader.h>
#include <Renderer/Resources/ResourceManager.h>

#include <GUI/ReflectionGUI.h>

#include <Core/Application.h>
#include <Core/Resources/ResourceManager.h>

#include <Utils/Reflection/Metadata.h>
#include <Utils/Serialization/SerializationUtils.h>

#include <imgui.h>

// clang-format off
RTTR_REGISTRATION
{
	using namespace GLEngine::Renderer;
	using namespace Utils::Reflection;

	rttr::registration::class_<C_StaticMeshHandles>("C_StaticMeshHandles")
		.constructor<>()(rttr::policy::ctor::as_std_shared_ptr)
		.property("Materials", &C_StaticMeshHandles::m_Materials)
		.property("ModelRes", &C_StaticMeshHandles::m_MeshResource)(
			rttr::policy::prop::as_reference_wrapper,
			RegisterMetamember<SerializationCls::MandatoryProperty>(true),
			RegisterMetaclass<MetaGUI::MeshResource>(),
			RegisterMetamember<UI::MeshResource::Name>("Model"),
			REGISTER_DEFAULT_VALUE(GLEngine::Core::ResourceHandle<MeshResource>()))
		.method("AfterDeserialize", &C_StaticMeshHandles::AfterDeserialize)()
		//.property("Shader", &C_StaticMesh::GetShader, &C_StaticMesh::SetShader)
		;
	rttr::type::register_wrapper_converter_for_base_classes<std::shared_ptr<C_StaticMeshHandles>>();
	rttr::type::register_converter_func([](std::shared_ptr<C_StaticMeshHandles> ptr, bool& ok) -> std::shared_ptr<GLEngine::Entity::I_Component> {
		ok = true;
		return std::static_pointer_cast<GLEngine::Entity::I_Component>(ptr);
	});
}
// clang-format on

namespace GLEngine::Renderer {

//=================================================================================
C_StaticMeshHandles::C_StaticMeshHandles()
	: I_RenderableComponent(nullptr)
{

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
		.shader =  "basicTracing",
		// .colorAttachmentFormat = GetTextureFormat(m_SwapChainImageFormat), no need for OpenGL and will be solved for Vulkan
	});
	}
	if (m_MeshResource.IsReady() && m_MeshResourceLive != m_MeshResource)
	{
		CleanRenderData();
		
		m_MeshResourceLive = m_MeshResource;

		const auto& scene = m_MeshResource.GetResource().GetScene();

		// if we are loading from new resource, there will be no materials loaded, so we will try to load them from the
		// source file, otherwise we don't want to do it, as there could be materials from the load level
		const bool loadMaterials = m_Materials.empty();

		std::vector<std::shared_ptr<C_Material>> materials;
		if (loadMaterials)
		{
			materials.reserve(scene.materials.size());
			for (const auto& material : scene.materials)
			{
				auto& mat = materials.emplace_back(C_MaterialManager::Instance().RegisterMaterial(C_Material(material)));
				if (material.textureIndex >= 0)
				{
					mat->SetColorMapPath(scene.textures[material.textureIndex]);
				}
				if (material.normalTextureIndex >= 0)
				{
					mat->SetNormalMapPath(scene.textures[material.normalTextureIndex]);
				}
			}
		}

		m_Meshes.reserve(scene.meshes.size());
		m_Materials.resize(scene.meshes.size());

		I_Renderer&		 renderer = Core::C_Application::Get().GetActiveRenderer();
		ResourceManager& rm		  = renderer.GetRM();
		for (const auto&& [idx, mesh] : scene.meshes | std::ranges::views::enumerate)
		{
			if (loadMaterials)
			{
				m_Materials[idx] = materials[mesh.materialIndex];
			}

			auto& meshContainer			  = m_Meshes.emplace_back();
			meshContainer.m_NumPrimitives = static_cast<uint32_t>(mesh.vertices.size());
			// load buffer

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
void C_StaticMeshHandles::CleanRenderData()
{
	I_Renderer&		 renderer = Core::C_Application::Get().GetActiveRenderer();
	ResourceManager& rm		  = renderer.GetRM();
	for (auto& meshContainer : m_Meshes)
	{
		rm.destroyBuffer(meshContainer.m_PositionsHandle);
		rm.destroyBuffer(meshContainer.m_NormalsHandle);
		rm.destroyBuffer(meshContainer.m_TexCoordsHandle);
		rm.destroyBuffer(meshContainer.m_TangentHandle);
		rm.destroyBuffer(meshContainer.m_BitangentHandle);
	}
	m_Meshes.clear();
}

//=================================================================================
void C_StaticMeshHandles::Render(Renderer3D& renderer) const
{
	if (m_MeshResource.IsReady())
	{
		for (const auto&& [meshContainer, material] : std::ranges::zip_view(m_Meshes, m_Materials))
		{
			renderer.Draw(RenderCall3D{
				.ModelMatrix   = GetComponentModelMatrix(),
				.NumPrimities  = meshContainer.m_NumPrimitives,
				.MaterialIndex = material->GetMaterialIndex(),
				.Buffers
				= {meshContainer.m_PositionsHandle, meshContainer.m_NormalsHandle, meshContainer.m_TexCoordsHandle, meshContainer.m_TangentHandle, meshContainer.m_BitangentHandle},
				.PipelineHandle = m_Pipeline,
			});
		}
	}
}

//=================================================================================
void C_StaticMeshHandles::SetMeshFile(const std::filesystem::path& meshFile)
{
	auto& rm   = Core::C_ResourceManager::Instance();
	auto  path = meshFile;
	if (meshFile.generic_string().find("Models") == std::string::npos)
		path = std::filesystem::path("Models") / meshFile;
	m_MeshResource = rm.LoadResource<Renderer::MeshResource>(path);
}

//=================================================================================
const std::filesystem::path& C_StaticMeshHandles::GetMeshFile() const
{
	// todo will fail if resource not loaded
	return m_MeshResource.GetResource().GetFilePath();
}

//=================================================================================
void C_StaticMeshHandles::DebugDrawGUI()
{
	if (::ImGui::CollapsingHeader("Materials"))
	{
		for (const auto& material : m_Materials)
		{
			material->DrawGUI();
		}
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
void C_StaticMeshHandles::AfterDeserialize(Utils::C_XMLDeserializer::DeserializeCtx& ctx)
{
	auto& materialManager = C_MaterialManager::Instance();
	for (const auto& material : m_Materials)
	{
		materialManager.RegisterMaterial(material);
	}
}

} // namespace GLEngine::Renderer