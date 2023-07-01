#include <GLRendererStdafx.h>

#include <GLRenderer/Components/StaticMeshHandles.h>
#include <GLRenderer/GLResourceManager.h>
#include <GLRenderer/OGLDevice.h>

#include <Renderer/IRenderer.h>
#include <Renderer/Mesh/Loading/ModelLoader.h>

#include <Core/Application.h>
#include <Core/Resources/LoadingQuery.h>
#include <Core/Resources/ResourceManager.h>

namespace GLEngine::GLRenderer {

//=================================================================================
C_StaticMeshHandles::C_StaticMeshHandles()
	: Renderer::I_RenderableComponent(nullptr)
{
	auto& rm	   = Core::C_ResourceManager::Instance();
	m_MeshResource = rm.LoadResource<Renderer::MeshResource>(R"(Models/sphere.obj)");
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
	if (m_Meshes.empty() == false && m_MeshResource.IsReady())
	{
		auto& scene = m_MeshResource.GetResource().GetScene();
		m_Meshes.reserve(scene.meshes.size());
		for (auto& mesh : scene.meshes)
		{
			auto& meshContainer = m_Meshes.emplace_back();
			// load buffer
			auto& mesh = m_MeshResource.GetResource().GetScene().meshes[0];
			// todo, the only place where I need OGL include
			Renderer::I_Renderer& renderer = Core::C_Application::Get().GetActiveRenderer();
			auto&				  device   = static_cast<C_GLDevice&>(renderer.GetDevice());

			const auto positionsSize		= static_cast<uint32_t>(sizeof(mesh.vertices[0]) * mesh.vertices.size());
			meshContainer.m_PositionsHandle = device.GetRM().createBuffer(Renderer::BufferDescriptor{
				.size  = positionsSize,
				.type  = Renderer::E_BufferType::Vertex,
				.usage = Renderer::E_ResourceUsage::Immutable,
			});
			renderer.SetBufferData(meshContainer.m_PositionsHandle, positionsSize, mesh.vertices.data());

			const auto normalsSize		  = static_cast<uint32_t>(sizeof(mesh.normals[0]) * mesh.normals.size());
			meshContainer.m_NormalsHandle = device.GetRM().createBuffer(Renderer::BufferDescriptor{
				.size  = normalsSize,
				.type  = Renderer::E_BufferType::Vertex,
				.usage = Renderer::E_ResourceUsage::Immutable,
			});
			renderer.SetBufferData(meshContainer.m_NormalsHandle, normalsSize, mesh.normals.data());

			const auto texCoordSize			= static_cast<uint32_t>(sizeof(mesh.texcoords[0]) * mesh.texcoords.size());
			meshContainer.m_TexCoordsHandle = device.GetRM().createBuffer(Renderer::BufferDescriptor{
				.size  = texCoordSize,
				.type  = Renderer::E_BufferType::Vertex,
				.usage = Renderer::E_ResourceUsage::Immutable,
			});
			renderer.SetBufferData(meshContainer.m_TexCoordsHandle, texCoordSize, mesh.texcoords.data());

			const auto tangentSize		  = static_cast<uint32_t>(sizeof(mesh.tangent[0]) * mesh.tangent.size());
			meshContainer.m_TangentHandle = device.GetRM().createBuffer(Renderer::BufferDescriptor{
				.size  = tangentSize,
				.type  = Renderer::E_BufferType::Vertex,
				.usage = Renderer::E_ResourceUsage::Immutable,
			});
			renderer.SetBufferData(meshContainer.m_TangentHandle, tangentSize, mesh.tangent.data());

			const auto bitangentSize		= static_cast<uint32_t>(sizeof(mesh.bitangent[0]) * mesh.bitangent.size());
			meshContainer.m_BitangentHandle = device.GetRM().createBuffer(Renderer::BufferDescriptor{
				.size  = bitangentSize,
				.type  = Renderer::E_BufferType::Vertex,
				.usage = Renderer::E_ResourceUsage::Immutable,
			});
			renderer.SetBufferData(meshContainer.m_BitangentHandle, bitangentSize, mesh.bitangent.data());
		}
	}
}

//=================================================================================
void C_StaticMeshHandles::Render(Renderer::Renderer3D& renderer) const
{
	if (m_MeshResource.IsReady())
	{
		for (auto& meshContainer : m_Meshes)
		{
			// needs pipeline as well
			renderer.Draw(Renderer::RenderCall3D{
				.ModelMatrix   = GetComponentModelMatrix(),
				.MaterialIndex = 0,
				.Buffers
				= {meshContainer.m_PositionsHandle, meshContainer.m_NormalsHandle, meshContainer.m_TexCoordsHandle, meshContainer.m_TangentHandle, meshContainer.m_BitangentHandle},
				.Shader = "handles",
			});
		}
	}
}

} // namespace GLEngine::GLRenderer