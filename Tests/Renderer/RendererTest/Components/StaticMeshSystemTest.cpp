#include <RendererTestStdafx.h>

#include <Renderer/Components/Mesh/StaticMeshComponents.h>
#include <Renderer/Materials/Material.h>
#include <Renderer/Materials/MaterialManager.h>
#include <Renderer/Mesh/Scene.h>
#include <Renderer/Systems/StaticMeshSystem.h>

#include <Entity/ComponentRegistry.h>
#include <Entity/Entity.h>
#include <Entity/World.h>

#include <CommonTestUtils/ResourceTestFixture.h>

#include "FakeRendererResources.h"

namespace GLEngine::Renderer {

// Grants MakeReadyMeshResource() below direct access to MeshResource::m_Scene, so a populated
// Scene can be attached in memory - see the friend declaration and comment in MeshResource.h.
class MeshResourceTestFixture {
public:
	static void SetScene(MeshResource& resource, std::shared_ptr<MeshData::Scene> scene) { resource.m_Scene = std::move(scene); }
};

namespace {

// Builds a MeshResource with one submesh (one triangle, all five vertex-attribute arrays
// populated so the system's buffer-size calculations are all non-degenerate) and marks it Ready
// without going through C_ResourceManager/disk I/O.
Core::ResourceHandle<MeshResource> MakeReadyMeshResource()
{
	auto  scene		   = std::make_shared<MeshData::Scene>();
	auto& mesh		   = scene->meshes.emplace_back();
	mesh.vertices	   = {glm::vec3(0.f), glm::vec3(1.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f)};
	mesh.normals	   = {glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 0.f, 1.f)};
	mesh.texcoords	   = {glm::vec2(0.f), glm::vec2(1.f, 0.f), glm::vec2(0.f, 1.f)};
	mesh.tangent	   = {glm::vec3(1.f, 0.f, 0.f), glm::vec3(1.f, 0.f, 0.f), glm::vec3(1.f, 0.f, 0.f)};
	mesh.bitangent	   = {glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, 1.f, 0.f)};
	mesh.materialIndex = 0;
	mesh.bbox.Add(glm::vec3(0.f));
	mesh.bbox.Add(glm::vec3(1.f));
	scene->bbox = mesh.bbox;

	auto resource = std::make_shared<MeshResource>();
	MeshResourceTestFixture::SetScene(*resource, scene);
	return Core::ResourceManagerBaseFixture::CreateResourceHandle(resource);
}

} // namespace

TEST(StaticMeshSystem, Update_BuildsGpuBuffersExactlyOnceForAReadyResource)
{
	Entity::C_World world;
	auto			entity = world.CreateEntity("Mesh");
	auto&			mesh   = entity.Add<S_StaticMesh>();
	mesh.requested		   = MakeReadyMeshResource();
	mesh.materials		   = {Core::ResourceHandle<MaterialResource>{}}; // non-empty: skip CopyMaterialsFromSourceFile (disk I/O)

	C_FakeResourceManager fakeRM;
	C_FakeRenderer		  fakeRenderer(fakeRM);
	StaticMeshSystem::RegisterDestroyHook(world, fakeRM);

	StaticMeshSystem::Update(world, fakeRenderer);

	ASSERT_TRUE(entity.Has<S_StaticMeshGPU>());
	EXPECT_EQ(fakeRM.createBufferCalls, 5); // positions/normals/texCoords/tangent/bitangent, one submesh
	EXPECT_EQ(fakeRM.createPipelineCalls, 1);

	const int createBufferCallsAfterFirstUpdate = fakeRM.createBufferCalls;
	StaticMeshSystem::Update(world, fakeRenderer);

	EXPECT_EQ(fakeRM.createBufferCalls, createBufferCallsAfterFirstUpdate); // live == requested: no rebuild
}

TEST(StaticMeshSystem, DestroyingTheGpuComponent_FreesAllBuffersAndThePipeline)
{
	Entity::C_World world;
	auto			entity = world.CreateEntity("Mesh");
	auto&			mesh   = entity.Add<S_StaticMesh>();
	mesh.requested		   = MakeReadyMeshResource();
	mesh.materials		   = {Core::ResourceHandle<MaterialResource>{}};

	C_FakeResourceManager fakeRM;
	C_FakeRenderer		  fakeRenderer(fakeRM);
	StaticMeshSystem::RegisterDestroyHook(world, fakeRM);
	StaticMeshSystem::Update(world, fakeRenderer);

	ASSERT_EQ(fakeRM.createBufferCalls, 5);
	ASSERT_EQ(fakeRM.destroyBufferCalls, 0);

	entity.Remove<S_StaticMeshGPU>();

	EXPECT_EQ(fakeRM.destroyBufferCalls, 5);
	EXPECT_EQ(fakeRM.destroyPipelineCalls, 1);
}

TEST(StaticMeshSystem, DestroyingTheEntity_UnregistersConvertedMaterials)
{
	Entity::C_World world;
	auto			entity = world.CreateEntity("Mesh");
	auto&			mesh   = entity.Add<S_StaticMesh>();

	auto&	   materialManager = C_MaterialManager::Instance();
	const auto material		   = materialManager.RegisterMaterial(C_Material("regression test material"));
	mesh.convertedMaterials	   = {material};

	C_FakeResourceManager fakeRM;
	StaticMeshSystem::RegisterDestroyHook(world, fakeRM);

	world.DestroyEntity(entity.Handle());
	world.OnUpdate();

	// UnregisterMaterial only erases the shared_ptr from the manager's internal list - `material`
	// (ours) keeps the object alive, so check reachability via ForEachMaterial rather than relying
	// on its (now-stale) index.
	bool stillRegistered = false;
	materialManager.ForEachMaterial([&](C_Material& m) {
		if (&m == material.get())
			stillRegistered = true;
	});
	EXPECT_FALSE(stillRegistered);
}

TEST(StaticMeshComponents, StaticMesh_IsRegisteredInComponentRegistry)
{
	EnsureStaticMeshComponentsRegistered();

	EXPECT_NE(Entity::C_ComponentRegistry::Instance().FindByName("StaticMesh"), nullptr);
}

} // namespace GLEngine::Renderer
