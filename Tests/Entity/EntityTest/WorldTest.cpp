#include <EntityTestStdafx.h>

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

} // namespace GLEngine::Entity
