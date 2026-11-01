#include <EntityTestStdafx.h>

#include <Entity/Components/CoreComponents.h>
#include <Entity/Entity.h>
#include <Entity/World.h>

#include <Core/GUID.h>

namespace GLEngine::Entity {

TEST(World, CreateEntity_AssignsNameAndValidGuid)
{
	C_World	 world;
	C_Entity entity = world.CreateEntity("TestEntity");

	EXPECT_EQ(entity.GetName(), "TestEntity");
	EXPECT_TRUE(entity.GetGuid().isValid());
}

TEST(World, CreateEntity_CreatesDuplicatelyNamedEntity)
{
	C_World world;

	C_Entity entity = world.CreateEntity("TestEntity");

	ASSERT_TRUE(entity.IsValid());
	EXPECT_EQ(entity.GetName(), "TestEntity");

	C_Entity duplicate = world.CreateEntity("TestEntity");

	ASSERT_FALSE(duplicate.IsValid()) << "Should not create duplicate entity";
}

TEST(World, CreateEntityWithGuid_CreatesDuplicatelyNamedEntity)
{
	C_World world;

	C_Entity entity = world.CreateEntityWithGuid(NextGUID(), "TestEntity");

	ASSERT_TRUE(entity.IsValid());
	EXPECT_EQ(entity.GetName(), "TestEntity");

	C_Entity duplicate = world.CreateEntityWithGuid(NextGUID(), "TestEntity");

	ASSERT_FALSE(duplicate.IsValid()) << "Should not create duplicate entity";
}

TEST(World, CreateEntityWithGuid_CreatesDuplicateGUIDEntity)
{
	C_World world;

	const auto guid	  = NextGUID();
	C_Entity   entity = world.CreateEntityWithGuid(guid, "TestEntity");

	ASSERT_TRUE(entity.IsValid());
	EXPECT_EQ(entity.GetName(), "TestEntity");

	C_Entity duplicate = world.CreateEntityWithGuid(guid, "DifferentEntity");

	ASSERT_FALSE(duplicate.IsValid()) << "Should not create duplicate entity";
}

TEST(World, CreateEntity_EmptyName)
{
	C_World world;

	C_Entity entity = world.CreateEntity("");

	ASSERT_FALSE(entity.IsValid());
}

TEST(World, FindByGuid_ReturnsCreatedEntity)
{
	C_World	 world;
	C_Entity created = world.CreateEntity("Named");

	C_Entity found = world.FindByGuid(created.GetGuid());

	ASSERT_TRUE(found.IsValid());
	EXPECT_EQ(found.GetGuid(), created.GetGuid());
}

TEST(World, FindByName_ReturnsCreatedEntity)
{
	C_World world;
	world.CreateEntity("Target");

	C_Entity found = world.FindByName("Target");

	ASSERT_TRUE(found.IsValid());
	EXPECT_EQ(found.GetName(), "Target");
}

TEST(World, FindByName_EmptyName)
{
	C_World world;

	C_Entity found = world.FindByName("");

	ASSERT_FALSE(found.IsValid());
}

TEST(World, FindByGuid_UnknownGuid_ReturnsInvalid)
{
	C_World world;
	world.CreateEntity("SomeEntity");

	EXPECT_FALSE(world.FindByGuid(GUID::INVALID_GUID).IsValid());
}

TEST(World, FindByName_UnknownName_ReturnsInvalid)
{
	C_World world;
	world.CreateEntity("SomeEntity");

	EXPECT_FALSE(world.FindByName("DoesNotExist").IsValid());
}

TEST(World, DestroyEntity_BeforeOnUpdate_StillFindableByGuid)
{
	C_World	   world;
	C_Entity   entity = world.CreateEntity("ToRemove");
	const GUID guid	  = entity.GetGuid();

	world.DestroyEntity(entity.Handle());

	// Destruction is deferred - the GUID lookup must still resolve until OnUpdate() flushes.
	EXPECT_TRUE(world.FindByGuid(guid).IsValid());
}

TEST(World, DestroyEntity_AfterOnUpdate_NoLongerFindable)
{
	C_World	   world;
	C_Entity   entity = world.CreateEntity("ToRemove");
	const GUID guid	  = entity.GetGuid();

	world.DestroyEntity(entity.Handle());
	world.OnUpdate();

	EXPECT_FALSE(world.FindByGuid(guid).IsValid());
	EXPECT_FALSE(world.FindByName("ToRemove").IsValid());
}

TEST(World, ClearLevel_RemovesAllEntities)
{
	C_World world;
	world.CreateEntity("A");
	world.CreateEntity("B");

	world.ClearLevel();

	EXPECT_FALSE(world.FindByName("A").IsValid());
	EXPECT_FALSE(world.FindByName("B").IsValid());
}

TEST(World, ClearLevel_ThenOnUpdate_SurvivesAStaleQueuedDestroy)
{
	C_World world;
	auto	a = world.CreateEntity("A");
	world.DestroyEntity(a.Handle()); // queued, not yet flushed by OnUpdate()

	world.ClearLevel();
	auto b = world.CreateEntity("A"); // same name as the queued-for-destroy entity
	world.OnUpdate();				  // flushes the stale queue entry alongside any real ones

	// Note: this passes regardless of whether ClearLevel() clears m_PendingDestroy, since
	// m_Registry.clear() already bumps entt's generation counters - OnUpdate()'s registry.valid()
	// guard rejects the stale handle either way. Kept as coverage for the suggested sequence
	// itself (queue a destroy, clear the level, reuse the name), not as a regression trap for
	// that one line.
	EXPECT_TRUE(world.FindByName("A").IsValid());
	EXPECT_EQ(world.FindByName("A").GetGuid(), b.GetGuid());
}

TEST(World, ClearLevel_ResetsFilename)
{
	C_World world;
	world.SetFilename("level.ent");
	ASSERT_FALSE(world.GetFilename().empty());

	world.ClearLevel();

	EXPECT_TRUE(world.GetFilename().empty());
}

TEST(World, GetOrCreateEntity_CreatesNewEntity)
{
	C_World world;

	C_Entity entity = world.GetOrCreateEntity("TestEntity");

	ASSERT_TRUE(entity.IsValid());
	EXPECT_EQ(entity.GetName(), "TestEntity");
}

TEST(World, GetOrCreateEntity_ReturnsExistingEntity)
{
	C_World world;

	C_Entity first	= world.GetOrCreateEntity("TestEntity");
	C_Entity second = world.GetOrCreateEntity("TestEntity");

	EXPECT_EQ(first.GetGuid(), second.GetGuid());
}

TEST(World, DestroyEntity_StaleHandle_DoesNotCrashAndLeavesOthersUnaffected)
{
	C_World			   world;
	C_Entity		   existing	   = world.CreateEntity("Existing");
	C_Entity		   toRemove	   = world.CreateEntity("ToRemove");
	const entt::entity staleHandle = toRemove.Handle();

	world.DestroyEntity(staleHandle);
	world.OnUpdate();

	// staleHandle now refers to an already-destroyed entity - destroying it again (e.g. a double
	// DestroyEntity call, or a dangling reference held past the first OnUpdate()) must not crash.
	EXPECT_NO_FATAL_FAILURE(world.DestroyEntity(staleHandle));
	EXPECT_NO_FATAL_FAILURE(world.OnUpdate());

	EXPECT_TRUE(world.FindByGuid(existing.GetGuid()).IsValid());
}

TEST(World, OnUpdate_DestroyingSomeEntities_LeavesExactlyTheSurvivorsLive)
{
	C_World world;
	auto	a = world.CreateEntity("A");
	auto	b = world.CreateEntity("B");
	auto	c = world.CreateEntity("C");
	auto	d = world.CreateEntity("D");

	// Every entity gets an S_IdentityComponent unconditionally on creation (see
	// CreateEntityWithGuid), so this view's size is a reliable live-entity count - C_World has no
	// dedicated count/enumeration API, so the test reaches through the public Registry() accessor.
	ASSERT_EQ(world.Registry().view<S_IdentityComponent>().size(), 4u);

	world.DestroyEntity(b.Handle());
	world.DestroyEntity(d.Handle());
	world.OnUpdate();

	EXPECT_EQ(world.Registry().view<S_IdentityComponent>().size(), 2u);
	EXPECT_TRUE(world.FindByGuid(a.GetGuid()).IsValid());
	EXPECT_TRUE(world.FindByGuid(c.GetGuid()).IsValid());
	EXPECT_FALSE(world.FindByGuid(b.GetGuid()).IsValid());
	EXPECT_FALSE(world.FindByGuid(d.GetGuid()).IsValid());
}

TEST(C_Entity, AddOrReplace_OverwritesAnExistingComponent)
{
	C_World world;
	auto	entity = world.CreateEntity("Test");

	// Every entity already has an S_TransformComponent from creation, so Add<S_TransformComponent>
	// here would assert - AddOrReplace is the explicit "overwrite if present" call.
	entity.Get<S_TransformComponent>().translation			= glm::vec3(1.f, 0.f, 0.f);
	entity.AddOrReplace<S_TransformComponent>().translation = glm::vec3(2.f, 3.f, 4.f);

	const auto& transform = entity.Get<S_TransformComponent>();
	EXPECT_FLOAT_EQ(transform.translation.x, 2.f);
	EXPECT_FLOAT_EQ(transform.translation.y, 3.f);
	EXPECT_FLOAT_EQ(transform.translation.z, 4.f);
}

TEST(C_Entity, Patch_MutatesComponentAndFiresOnUpdateSignal)
{
	// F-004: Get() returns a mutable T& with no way to fire registry's on_update<T> signal -
	// Patch() is the entity-level equivalent of World()->Registry().patch<T>(Handle(), ...) that
	// does. Confirmed here via S_DirtyTransformTag, the exact signal C_World's constructor wires
	// up for S_TransformComponent - mutating through Get() alone would not set it.
	C_World world;
	auto	entity = world.CreateEntity("Test");
	entity.Remove<S_DirtyTransformTag>(); // creation unconditionally seeds one; start from a clean slate

	entity.Patch<S_TransformComponent>([](S_TransformComponent& t) { t.translation = glm::vec3(3.f, 4.f, 5.f); });

	EXPECT_EQ(entity.Get<S_TransformComponent>().translation, glm::vec3(3.f, 4.f, 5.f));
	EXPECT_TRUE(entity.Has<S_DirtyTransformTag>());
}

} // namespace GLEngine::Entity
