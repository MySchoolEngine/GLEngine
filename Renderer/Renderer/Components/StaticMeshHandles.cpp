#include <RendererStdafx.h>

#include <Renderer/Components/StaticMeshHandles.h>
#include <Renderer/IRenderer.h>
#include <Renderer/Mesh/Loading/ModelLoader.h>
#include <Renderer/Resources/ResourceManager.h>

#include <Core/Application.h>
#include <Core/Resources/LoadingQuery.h>
#include <Core/Resources/ResourceManager.h>

namespace GLEngine::Renderer {

//=================================================================================
C_StaticMeshHandles::C_StaticMeshHandles()
	: I_RenderableComponent(nullptr)
{
	auto& rm	   = Core::C_ResourceManager::Instance();
	m_MeshResource = rm.LoadResource<MeshResource>(R"(Models/sphere.obj)");
	m_Pipeline = Core::C_Application::Get().GetActiveRenderer().GetRM().createPipeline(PipelineDescriptor{
		.primitiveType = Renderer::E_RenderPrimitives::TriangleList,
		.bindingCount = 5,
		.vertexInput   = {{
							  .binding = 0,
							  .type	   = Renderer::T_TypeShaderDataType_v<glm::vec4>
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
						  },},
		.shader =  "handles",
		// .colorAttachementFormat = GetTextureFormat(m_SwapChainImageFormat), no need for OpenGL and will be solved for Vulkan
	});
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
	if (m_Meshes.empty() == true && m_MeshResource.IsReady())
	{
		auto& scene = m_MeshResource.GetResource().GetScene();
		m_Meshes.reserve(scene.meshes.size());
		for (auto& mesh : scene.meshes)
		{
			auto& meshContainer			  = m_Meshes.emplace_back();
			meshContainer.m_NumPrimitives = static_cast<uint32_t>(mesh.vertices.size());
			// load buffer
			auto& mesh = m_MeshResource.GetResource().GetScene().meshes[0];

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
	if (m_MeshResource.IsReady())
	{
		for (auto& meshContainer : m_Meshes)
		{
			// needs pipeline as well
			renderer.Draw(RenderCall3D{
				.ModelMatrix   = GetComponentModelMatrix(),
				.NumPrimities  = meshContainer.m_NumPrimitives,
				.MaterialIndex = 0,
				.Buffers
				= {meshContainer.m_PositionsHandle, meshContainer.m_NormalsHandle, meshContainer.m_TexCoordsHandle, meshContainer.m_TangentHandle, meshContainer.m_BitangentHandle},
				.Pipeline = m_Pipeline,
			});
		}
	}
}

} // namespace GLEngine::Renderer