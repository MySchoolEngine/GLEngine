#include <RendererTestStdafx.h>

#include <Renderer/Components/Mesh/StaticMeshComponents.h>
#include <Renderer/IRenderer.h>
#include <Renderer/Mesh/Scene.h>
#include <Renderer/Resources/ResourceManager.h>
#include <Renderer/Systems/StaticMeshSystem.h>

#include <Entity/ComponentRegistry.h>
#include <Entity/Entity.h>
#include <Entity/World.h>

#include <CommonTestUtils/ResourceTestFixture.h>

namespace GLEngine::Renderer {

// Grants MakeReadyMeshResource() below direct access to MeshResource::m_Scene, so a populated
// Scene can be attached in memory - see the friend declaration and comment in MeshResource.h.
class MeshResourceTestFixture {
public:
	static void SetScene(MeshResource& resource, std::shared_ptr<MeshData::Scene> scene) { resource.m_Scene = std::move(scene); }
};

namespace {

// Trivial placeholder concrete-resource type for ResourcePool's slot_map - the test never
// inspects what's stored, only that create/destroy are balanced and return distinct handles.
struct S_FakeGpuResource {};

class C_FakeResourceManager final : public ResourceManager {
public:
	int createBufferCalls	 = 0;
	int destroyBufferCalls	 = 0;
	int createPipelineCalls	 = 0;
	int destroyPipelineCalls = 0;

	Handle<Shader> createShader(const std::filesystem::path&) override { return {}; }
	void		   destoryShader(Handle<Shader>) override {}

	Handle<Pipeline> createPipeline(const PipelineDescriptor&) override
	{
		++createPipelineCalls;
		return m_Pipelines.CreateNew();
	}
	void destoryPipeline(Handle<Pipeline> handle) override
	{
		++destroyPipelineCalls;
		m_Pipelines.RemoveHandle(handle);
	}

	Handle<Texture> createTexture(const TextureDescriptor&) override { return {}; }
	void			destoryTexture(Handle<Texture>) override {}

	Handle<Buffer> createBuffer(const BufferDescriptor&) override
	{
		++createBufferCalls;
		return m_Buffers.CreateNew();
	}
	void destroyBuffer(const Handle<Buffer>& handle) override
	{
		++destroyBufferCalls;
		m_Buffers.RemoveHandle(handle);
	}

	Handle<Sampler> createSampler(const SamplerDescriptor2D&) override { return {}; }
	void			destroySampler(const Handle<Sampler>&) override {}

private:
	ResourcePool<Buffer, S_FakeGpuResource>	  m_Buffers;
	ResourcePool<Pipeline, S_FakeGpuResource> m_Pipelines;
};

class C_FakeRenderer final : public I_Renderer {
public:
	explicit C_FakeRenderer(ResourceManager& rm)
		: m_RM(rm)
	{
	}

	void Lock(bool) override {}
	void AddCommand(T_CommandPtr) override {}
	void AddTransferCommand(T_CommandPtr) override {}
	void Commit() const override {}
	void ClearCommandBuffers() override {}
	void SetBufferData(Handle<Buffer>, std::size_t, const void*) override {}
	void SetBufferSubData(Handle<Buffer>, std::size_t, std::size_t, const void*) override {}
	void SetTextureSampler(Handle<Texture>, Handle<Sampler>) override {}

	// Never exercised by StaticMeshSystem - no fake I_Device exists to back this.
	I_Device& GetDevice() override
	{
		GLE_ASSERT(false, "C_FakeRenderer::GetDevice is not implemented; nothing in this test should call it");
		return *static_cast<I_Device*>(nullptr);
	}
	ResourceManager& GetRM() override { return m_RM; }

	E_PassType GetCurrentPassType() const override { return E_PassType::FinalPass; }

private:
	ResourceManager& m_RM;
};

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

TEST(StaticMeshComponents, StaticMesh_IsRegisteredInComponentRegistry)
{
	EnsureStaticMeshComponentsRegistered();

	EXPECT_NE(Entity::C_ComponentRegistry::Instance().FindByName("StaticMesh"), nullptr);
}

} // namespace GLEngine::Renderer
