#include <RendererStdafx.h>

#include <Renderer/Components/Mesh/StaticMeshComponents.h>
#include <Renderer/IRenderer.h>
#include <Renderer/Materials/Material.h>
#include <Renderer/Materials/MaterialManager.h>
#include <Renderer/Materials/MeshMaterialExtractor.h>
#include <Renderer/Materials/PBRMaterialData.h>
#include <Renderer/Mesh/Scene.h>
#include <Renderer/Renderer3D.h>
#include <Renderer/Resources/ResourceManager.h>
#include <Renderer/Systems/StaticMeshSystem.h>

#include <Entity/Components/CoreComponents.h>
#include <Entity/World.h>

#include <Utils/StdVectorUtils.h>

namespace GLEngine::Renderer::StaticMeshSystem {

namespace {

ResourceManager* g_ResourceManagerForDestroy = nullptr;

void FreeGPUBuffers(entt::registry& registry, entt::entity e)
{
	GLE_ASSERT(g_ResourceManagerForDestroy != nullptr, "RegisterDestroyHook must be called before any S_StaticMeshGPU is destroyed");
	auto& gpu = registry.get<S_StaticMeshGPU>(e);
	for (auto& submesh : gpu.submeshes)
	{
		g_ResourceManagerForDestroy->destroyBuffer(submesh.positions);
		g_ResourceManagerForDestroy->destroyBuffer(submesh.normals);
		g_ResourceManagerForDestroy->destroyBuffer(submesh.texCoords);
		g_ResourceManagerForDestroy->destroyBuffer(submesh.tangent);
		g_ResourceManagerForDestroy->destroyBuffer(submesh.bitangent);
	}
	g_ResourceManagerForDestroy->destoryPipeline(gpu.pipeline);
}

// Ported verbatim from the old C_StaticMeshHandles::ConvertHandleToMaterial.
std::shared_ptr<C_Material> ConvertHandleToMaterial(const Core::ResourceHandle<MaterialResource>& handle)
{
	auto& materialManager = C_MaterialManager::Instance();
	if (!handle.IsReady())
	{
		return nullptr;
	}

	const auto& materialResource = handle.GetResource();
	auto		material		 = C_Material(materialResource.GetMaterialName());
	if (const auto* pbrData = dynamic_cast<const C_PBRMaterialData*>(materialResource.GetMaterialData()))
	{
		material.SetDiffuseColor(pbrData->GetColour());
		material.SetRoughness(pbrData->GetRoughness());
		if (pbrData->GetColorMapRes())
			material.SetColorMapPath(pbrData->GetColorMapRes().GetResource().GetFilePath());
		if (pbrData->GetNormalMapRes())
			material.SetNormalMapPath(pbrData->GetNormalMapRes().GetResource().GetFilePath());
		if (pbrData->GetRoughnessMapRes())
			material.SetRoughnessMapPath(pbrData->GetRoughnessMapRes().GetResource().GetFilePath());
	}
	return materialManager.RegisterMaterial(std::move(material));
}

// Ported from C_StaticMeshHandles::CopyMaterialsFromSourceFile + ConvertHandlesToMaterials.
void UpdateMaterials(S_StaticMesh& mesh)
{
	if (mesh.convertedMaterials.empty() == false && !::Utils::contains(mesh.convertedMaterials, nullptr))
		return;

	if (mesh.materials.empty())
	{
		const auto	materials = ExtractMaterialsFromMesh(mesh.requested.GetResource());
		const auto& scene	  = mesh.requested.GetResource().GetScene();
		mesh.materials.assign(scene.meshes.size(), Core::ResourceHandle<MaterialResource>{});
		for (std::size_t i = 0; i < scene.meshes.size(); ++i)
		{
			mesh.materials[i] = materials[scene.meshes[i].materialIndex];
		}
	}

	auto& materialManager = C_MaterialManager::Instance();
	for (auto& material : mesh.convertedMaterials)
	{
		materialManager.UnregisterMaterial(material);
	}
	mesh.convertedMaterials.assign(mesh.materials.size(), nullptr);
	for (std::size_t i = 0; i < mesh.materials.size(); ++i)
	{
		mesh.convertedMaterials[i] = ConvertHandleToMaterial(mesh.materials[i]);
	}
}

} // namespace

//=================================================================================
void RegisterDestroyHook(Entity::C_World& world, ResourceManager& resourceManager)
{
	g_ResourceManagerForDestroy = &resourceManager;
	world.Registry().on_destroy<S_StaticMeshGPU>().connect<&FreeGPUBuffers>();
}

//=================================================================================
void Update(Entity::C_World& world, I_Renderer& renderer)
{
	auto& registry = world.Registry();
	auto& rm	   = renderer.GetRM();

	for (auto [e, mesh] : registry.view<S_StaticMesh>().each())
	{
		if (mesh.requested.IsReady() && mesh.live != mesh.requested)
		{
			// Triggers on_destroy<S_StaticMeshGPU> -> FreeGPUBuffers for any previous buffers.
			if (registry.all_of<S_StaticMeshGPU>(e))
				registry.remove<S_StaticMeshGPU>(e);

			auto&		gpu	  = registry.emplace<S_StaticMeshGPU>(e);
			const auto& scene = mesh.requested.GetResource().GetScene();

			GLE_TODO("6-11-2024", "RohacekD", "Can we validate the definition with shaders?");
			gpu.pipeline = rm.createPipeline(PipelineDescriptor{
				.primitiveType = Renderer::E_RenderPrimitives::TriangleList,
				.bindingCount  = 5,
				.vertexInput   = {{.binding = 0, .type = Renderer::T_TypeShaderDataType_v<glm::vec3>},
								  {.binding = 1, .type = Renderer::T_TypeShaderDataType_v<glm::vec3>},
								  {.binding = 2, .type = Renderer::T_TypeShaderDataType_v<glm::vec2>},
								  {.binding = 3, .type = Renderer::T_TypeShaderDataType_v<glm::vec3>},
								  {.binding = 4, .type = Renderer::T_TypeShaderDataType_v<glm::vec3>}},
				.shader		   = "basicTracing",
			});

			gpu.submeshes.reserve(scene.meshes.size());
			for (const auto& subMesh : scene.meshes)
			{
				auto& submeshGpu		 = gpu.submeshes.emplace_back();
				submeshGpu.numPrimitives = static_cast<std::uint32_t>(subMesh.vertices.size());

				const auto positionsSize = static_cast<std::uint32_t>(sizeof(subMesh.vertices[0]) * subMesh.vertices.size());
				submeshGpu.positions	 = rm.createBuffer(BufferDescriptor{.size = positionsSize, .type = E_BufferType::Vertex, .usage = E_ResourceUsage::Immutable});
				renderer.SetBufferData(submeshGpu.positions, positionsSize, subMesh.vertices.data());

				const auto normalsSize = static_cast<std::uint32_t>(sizeof(subMesh.normals[0]) * subMesh.normals.size());
				submeshGpu.normals	   = rm.createBuffer(BufferDescriptor{.size = normalsSize, .type = E_BufferType::Vertex, .usage = E_ResourceUsage::Immutable});
				renderer.SetBufferData(submeshGpu.normals, normalsSize, subMesh.normals.data());

				const auto texCoordSize = static_cast<std::uint32_t>(sizeof(subMesh.texcoords[0]) * subMesh.texcoords.size());
				submeshGpu.texCoords	= rm.createBuffer(BufferDescriptor{.size = texCoordSize, .type = E_BufferType::Vertex, .usage = E_ResourceUsage::Immutable});
				renderer.SetBufferData(submeshGpu.texCoords, texCoordSize, subMesh.texcoords.data());

				const auto tangentSize = static_cast<std::uint32_t>(sizeof(subMesh.tangent[0]) * subMesh.tangent.size());
				submeshGpu.tangent	   = rm.createBuffer(BufferDescriptor{.size = tangentSize, .type = E_BufferType::Vertex, .usage = E_ResourceUsage::Immutable});
				renderer.SetBufferData(submeshGpu.tangent, tangentSize, subMesh.tangent.data());

				const auto bitangentSize = static_cast<std::uint32_t>(sizeof(subMesh.bitangent[0]) * subMesh.bitangent.size());
				submeshGpu.bitangent	 = rm.createBuffer(BufferDescriptor{.size = bitangentSize, .type = E_BufferType::Vertex, .usage = E_ResourceUsage::Immutable});
				renderer.SetBufferData(submeshGpu.bitangent, bitangentSize, subMesh.bitangent.data());
			}

			mesh.live = mesh.requested;

			registry.emplace_or_replace<Entity::S_LocalBounds>(e, Entity::S_LocalBounds{scene.bbox});
		}

		if (mesh.requested.IsReady())
		{
			UpdateMaterials(mesh);
		}
	}
}

//=================================================================================
void Render(const S_StaticMesh& mesh, const S_StaticMeshGPU& gpu, const Entity::S_WorldTransformComponent& worldTransform, Renderer3D& renderer3D)
{
	auto& materialManager = C_MaterialManager::Instance();

	for (const auto&& [submesh, material] : std::ranges::zip_view(gpu.submeshes, mesh.convertedMaterials))
	{
		renderer3D.Draw(RenderCall3D{
			.ModelMatrix	= worldTransform.world,
			.NumPrimities	= submesh.numPrimitives,
			.MaterialIndex	= material ? material->GetMaterialIndex() : materialManager.GetErrorMaterial()->GetMaterialIndex(),
			.Buffers		= {submesh.positions, submesh.normals, submesh.texCoords, submesh.tangent, submesh.bitangent},
			.PipelineHandle = gpu.pipeline,
		});
	}
}

} // namespace GLEngine::Renderer::StaticMeshSystem
