#include <EntityTestStdafx.h>

#include <Entity/BasicEntity.h>
#include <Entity/EntityManager.h>
#include <Core/GUID.h>

namespace GLEngine::Entity {

//=================================================================================
// Creation tests
//=================================================================================

TEST(EntityManager, GetOrCreateEntity_CreatesNewEntity)
{
	C_EntityManager manager;
	const auto entity = manager.GetOrCreateEntity("TestEntity");

	ASSERT_NE(entity, nullptr);
	EXPECT_EQ(entity->GetName(), "TestEntity");
}

TEST(EntityManager, GetOrCreateEntity_ReturnsExistingEntity)
{
	C_EntityManager manager;
	const auto first = manager.GetOrCreateEntity("TestEntity");
	const auto second = manager.GetOrCreateEntity("TestEntity");

	EXPECT_EQ(first, second);
}

TEST(EntityManager, AddEntity_IncreasesCount)
{
	C_EntityManager manager;
	EXPECT_EQ(manager.GetEntities().size(), 0u);

	manager.AddEntity(std::make_shared<C_BasicEntity>("A"));
	EXPECT_EQ(manager.GetEntities().size(), 1u);

	manager.AddEntity(std::make_shared<C_BasicEntity>("B"));
	EXPECT_EQ(manager.GetEntities().size(), 2u);
}

TEST(EntityManager, GetEntities_ReturnsAllAdded)
{
	C_EntityManager manager;
	manager.AddEntity(std::make_shared<C_BasicEntity>("A"));
	manager.AddEntity(std::make_shared<C_BasicEntity>("B"));
	manager.AddEntity(std::make_shared<C_BasicEntity>("C"));

	EXPECT_EQ(manager.GetEntities().size(), 3u);
}

TEST(EntityManager, GetEntityByGUID_ReturnsCorrectEntity)
{
	C_EntityManager manager;
	auto            entity = std::make_shared<C_BasicEntity>("Named");
	manager.AddEntity(entity);

	const GUID id = entity->GetID();
	const auto found = manager.GetEntity(id);

	ASSERT_NE(found, nullptr);
	EXPECT_EQ(found->GetID(), id);
}

TEST(EntityManager, GetEntityByName_ReturnsCorrectEntity)
{
	C_EntityManager manager;
	manager.AddEntity(std::make_shared<C_BasicEntity>("Target"));

	const auto found = manager.GetEntity("Target");

	ASSERT_NE(found, nullptr);
	EXPECT_EQ(found->GetName(), "Target");
}

TEST(EntityManager, GetEntityByInvalidGUID_ReturnsNull)
{
	C_EntityManager manager;
	manager.AddEntity(std::make_shared<C_BasicEntity>("SomeEntity"));

	EXPECT_EQ(manager.GetEntity(GUID::INVALID_GUID), nullptr);
}

TEST(EntityManager, GetEntityByUnknownName_ReturnsNull)
{
	C_EntityManager manager;
	manager.AddEntity(std::make_shared<C_BasicEntity>("SomeEntity"));

	EXPECT_EQ(manager.GetEntity("DoesNotExist"), nullptr);
}

//=================================================================================
// Deletion tests
//=================================================================================

TEST(EntityManager, RemoveEntity_BeforeOnUpdate_EntityStillExists)
{
	C_EntityManager manager;
	auto            entity = std::make_shared<C_BasicEntity>("ToRemove");
	manager.AddEntity(entity);

	manager.RemoveEntity(entity->GetID());

	// Removal is deferred — entity must still be present until OnUpdate()
	EXPECT_EQ(manager.GetEntities().size(), 1u);
	EXPECT_NE(manager.GetEntity(entity->GetID()), nullptr);
}

TEST(EntityManager, RemoveEntity_AfterOnUpdate_EntityIsRemoved)
{
	C_EntityManager manager;
	auto            entity = std::make_shared<C_BasicEntity>("ToRemove");
	manager.AddEntity(entity);
	const GUID id = entity->GetID();

	manager.RemoveEntity(id);
	manager.OnUpdate();

	EXPECT_EQ(manager.GetEntities().size(), 0u);
	EXPECT_EQ(manager.GetEntity(id), nullptr);
}

TEST(EntityManager, ClearLevel_RemovesAllEntities)
{
	C_EntityManager manager;
	manager.AddEntity(std::make_shared<C_BasicEntity>("A"));
	manager.AddEntity(std::make_shared<C_BasicEntity>("B"));
	manager.AddEntity(std::make_shared<C_BasicEntity>("C"));

	manager.ClearLevel();

	EXPECT_EQ(manager.GetEntities().size(), 0u);
}

TEST(EntityManager, RemoveEntity_UnknownGUID_DoesNotCrash)
{
	C_EntityManager manager;
	manager.AddEntity(std::make_shared<C_BasicEntity>("Existing"));

	const GUID unknownId = NextGUID();
	EXPECT_NO_FATAL_FAILURE(manager.RemoveEntity(unknownId));
	EXPECT_NO_FATAL_FAILURE(manager.OnUpdate());

	// Original entity must be unaffected
	EXPECT_EQ(manager.GetEntities().size(), 1u);
}

} // namespace GLEngine::Entity
