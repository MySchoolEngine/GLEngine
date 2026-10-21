#include <RendererTestStdafx.h>

#include <Renderer/Components/Mesh/GeomComponents.h>
#include <Renderer/IRenderer.h>
#include <Renderer/Resources/ResourceManager.h>
#include <Renderer/Systems/GeomSystem.h>

#include <Entity/ComponentRegistry.h>
#include <Entity/Entity.h>
#include <Entity/World.h>

namespace GLEngine::Renderer {

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

	// Never exercised by GeomSystem - no fake I_Device exists to back this.
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

} // namespace

TEST(GeomSystem, Update_BuildsGpuBuffersExactlyOnceAndAssignsADefaultMaterial)
{
	Entity::C_World world;
	auto			entity = world.CreateEntity("Geom");
	entity.Add<S_GeomComponent>();

	C_FakeResourceManager fakeRM;
	C_FakeRenderer		  fakeRenderer(fakeRM);
	GeomSystem::RegisterDestroyHook(world, fakeRM);

	GeomSystem::Update(world, fakeRenderer);

	ASSERT_TRUE(entity.Has<S_GeomGPU>());
	EXPECT_NE(entity.Get<S_GeomComponent>().material, nullptr);
	EXPECT_EQ(fakeRM.createBufferCalls, 5); // positions/normals/texCoords/tangent/bitangent
	EXPECT_EQ(fakeRM.createPipelineCalls, 1);

	const int createBufferCallsAfterFirstUpdate = fakeRM.createBufferCalls;
	GeomSystem::Update(world, fakeRenderer);

	EXPECT_EQ(fakeRM.createBufferCalls, createBufferCallsAfterFirstUpdate); // already has S_GeomGPU: no rebuild
}

TEST(GeomSystem, DestroyingTheGpuComponent_FreesAllBuffersAndThePipeline)
{
	Entity::C_World world;
	auto			entity = world.CreateEntity("Geom");
	entity.Add<S_GeomComponent>();

	C_FakeResourceManager fakeRM;
	C_FakeRenderer		  fakeRenderer(fakeRM);
	GeomSystem::RegisterDestroyHook(world, fakeRM);
	GeomSystem::Update(world, fakeRenderer);

	ASSERT_EQ(fakeRM.createBufferCalls, 5);
	ASSERT_EQ(fakeRM.destroyBufferCalls, 0);

	entity.Remove<S_GeomGPU>();

	EXPECT_EQ(fakeRM.destroyBufferCalls, 5);
	EXPECT_EQ(fakeRM.destroyPipelineCalls, 1);
}

TEST(GeomComponents, Geometry_IsRegisteredInComponentRegistry)
{
	EnsureGeomComponentsRegistered();

	EXPECT_NE(Entity::C_ComponentRegistry::Instance().FindByName("Geometry"), nullptr);
}

} // namespace GLEngine::Renderer
