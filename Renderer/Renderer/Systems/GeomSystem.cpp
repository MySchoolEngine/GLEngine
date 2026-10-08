#include <RendererStdafx.h>

#include <Renderer/Components/Mesh/GeomComponents.h>
#include <Renderer/IRenderer.h>
#include <Renderer/Materials/Material.h>
#include <Renderer/Materials/MaterialManager.h>
#include <Renderer/Mesh/Geometry.h>
#include <Renderer/Mesh/Scene.h>
#include <Renderer/Resources/ResourceManager.h>
#include <Renderer/Systems/GeomSystem.h>

#include <Entity/Components/CoreComponents.h>
#include <Entity/World.h>

namespace GLEngine::Renderer::GeomSystem {

namespace {

ResourceManager* g_ResourceManagerForDestroy = nullptr;

void FreeGPUBuffers(entt::registry& registry, entt::entity e)
{
	GLE_ASSERT(g_ResourceManagerForDestroy != nullptr, "RegisterDestroyHook must be called before any S_GeomGPU is destroyed");
	auto& gpu = registry.get<S_GeomGPU>(e);
	g_ResourceManagerForDestroy->destroyBuffer(gpu.positions);
	g_ResourceManagerForDestroy->destroyBuffer(gpu.normals);
	g_ResourceManagerForDestroy->destroyBuffer(gpu.texCoords);
	g_ResourceManagerForDestroy->destroyBuffer(gpu.tangent);
	g_ResourceManagerForDestroy->destroyBuffer(gpu.bitangent);
	g_ResourceManagerForDestroy->destoryPipeline(gpu.pipeline);
}

} // namespace

//=================================================================================
void RegisterDestroyHook(Entity::C_World& world, ResourceManager& resourceManager)
{
	g_ResourceManagerForDestroy = &resourceManager;
	world.Registry().on_destroy<S_GeomGPU>().connect<&FreeGPUBuffers>();
}

//=================================================================================
void Update(Entity::C_World& world, I_Renderer& renderer)
{
	auto& registry = world.Registry();
	auto& rm	   = renderer.GetRM();

	for (auto [e, component] : registry.view<S_GeomComponent>().each())
	{
		if (!component.material)
		{
			auto& materialManager = C_MaterialManager::Instance();
			component.material	  = materialManager.GetMaterial("GeomComponent");
			if (!component.material)
			{
				component.material = materialManager.RegisterMaterial(C_Material("GeomComponent"));
			}
		}

		if (registry.all_of<S_GeomGPU>(e))
			continue;

		const auto mesh = MeshData::C_Geometry::CreatePlane();
		auto&	   gpu	= registry.emplace<S_GeomGPU>(e);

		GLE_TODO("6-11-2024", "RohacekD", "Can we validate the definition with shaders?");
		gpu.pipeline = rm.createPipeline(PipelineDescriptor{
			.primitiveType = E_RenderPrimitives::TriangleList,
			.bindingCount  = 5,
			.vertexInput   = {{.binding = 0, .type = Renderer::T_TypeShaderDataType_v<glm::vec3>},
							  {.binding = 1, .type = Renderer::T_TypeShaderDataType_v<glm::vec3>},
							  {.binding = 2, .type = Renderer::T_TypeShaderDataType_v<glm::vec2>},
							  {.binding = 3, .type = Renderer::T_TypeShaderDataType_v<glm::vec3>},
							  {.binding = 4, .type = Renderer::T_TypeShaderDataType_v<glm::vec3>}},
			.shader		   = "basicTracing",
		});

		gpu.numPrimitives = static_cast<std::uint32_t>(mesh.vertices.size());

		const auto positionsSize = static_cast<std::uint32_t>(sizeof(mesh.vertices[0]) * mesh.vertices.size());
		gpu.positions			 = rm.createBuffer(BufferDescriptor{.size = positionsSize, .type = E_BufferType::Vertex, .usage = E_ResourceUsage::Immutable});
		renderer.SetBufferData(gpu.positions, positionsSize, mesh.vertices.data());

		const auto normalsSize = static_cast<std::uint32_t>(sizeof(mesh.normals[0]) * mesh.normals.size());
		gpu.normals			   = rm.createBuffer(BufferDescriptor{.size = normalsSize, .type = E_BufferType::Vertex, .usage = E_ResourceUsage::Immutable});
		renderer.SetBufferData(gpu.normals, normalsSize, mesh.normals.data());

		const auto texCoordSize = static_cast<std::uint32_t>(sizeof(mesh.texcoords[0]) * mesh.texcoords.size());
		gpu.texCoords			= rm.createBuffer(BufferDescriptor{.size = texCoordSize, .type = E_BufferType::Vertex, .usage = E_ResourceUsage::Immutable});
		renderer.SetBufferData(gpu.texCoords, texCoordSize, mesh.texcoords.data());

		const auto tangentSize = static_cast<std::uint32_t>(sizeof(mesh.tangent[0]) * mesh.tangent.size());
		gpu.tangent			   = rm.createBuffer(BufferDescriptor{.size = tangentSize, .type = E_BufferType::Vertex, .usage = E_ResourceUsage::Immutable});
		renderer.SetBufferData(gpu.tangent, tangentSize, mesh.tangent.data());

		const auto bitangentSize = static_cast<std::uint32_t>(sizeof(mesh.bitangent[0]) * mesh.bitangent.size());
		gpu.bitangent			 = rm.createBuffer(BufferDescriptor{.size = bitangentSize, .type = E_BufferType::Vertex, .usage = E_ResourceUsage::Immutable});
		renderer.SetBufferData(gpu.bitangent, bitangentSize, mesh.bitangent.data());

		registry.emplace_or_replace<Entity::S_LocalBounds>(e, Entity::S_LocalBounds{mesh.bbox});
	}
}

} // namespace GLEngine::Renderer::GeomSystem
