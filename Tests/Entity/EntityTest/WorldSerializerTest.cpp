#include <EntityTestStdafx.h>

#include <Entity/ComponentRegistry.h>
#include <Entity/Components/CoreComponents.h>
#include <Entity/Entity.h>
#include <Entity/Serialization/WorldSerializer.h>
#include <Entity/World.h>

#include <Core/Resources/LoadingQuery.h>
#include <Core/Resources/ResourceManager.h>

#include <pugixml.hpp>

namespace GLEngine::Entity {

TEST(WorldSerializer, SaveThenLoad_PreservesGuidNameAndTransform)
{
	C_World world;
	auto	entity = world.CreateEntity("floor");

	// Non-trivial on every axis and every field (translation x/y/z, a non-identity rotation, and
	// a non-uniform scale), so a serializer bug that drops or mis-orders a field moves a value
	// to the wrong place instead of coincidentally passing (e.g. the old test's translation=(0,
	// -1.5,0) couldn't tell an x/y swap from a correct round-trip).
	auto& transform		= entity.Get<S_TransformComponent>();
	transform.translation = glm::vec3(3.25f, -1.5f, 7.75f);
	transform.rotation	   = glm::angleAxis(glm::radians(45.f), glm::vec3(0.f, 1.f, 0.f));
	transform.scale		   = glm::vec3(2.f, 0.5f, 3.f);
	const GUID originalGuid = entity.GetGuid();

	C_WorldSerializer  serializer;
	pugi::xml_document doc = serializer.Save(world);

	C_World			   loaded;
	Core::LoadingQuery query;
	serializer.Load(doc, loaded, Core::C_ResourceManager::Instance(), query, /*loadHandlesInstantly=*/true);

	C_Entity found = loaded.FindByGuid(originalGuid);
	ASSERT_TRUE(found.IsValid());
	EXPECT_EQ(found.GetName(), "floor");

	const auto& loaded_transform = found.Get<S_TransformComponent>();
	EXPECT_NEAR(loaded_transform.translation.x, transform.translation.x, 0.0001f);
	EXPECT_NEAR(loaded_transform.translation.y, transform.translation.y, 0.0001f);
	EXPECT_NEAR(loaded_transform.translation.z, transform.translation.z, 0.0001f);
	EXPECT_NEAR(loaded_transform.rotation.x, transform.rotation.x, 0.0001f);
	EXPECT_NEAR(loaded_transform.rotation.y, transform.rotation.y, 0.0001f);
	EXPECT_NEAR(loaded_transform.rotation.z, transform.rotation.z, 0.0001f);
	EXPECT_NEAR(loaded_transform.rotation.w, transform.rotation.w, 0.0001f);
	EXPECT_NEAR(loaded_transform.scale.x, transform.scale.x, 0.0001f);
	EXPECT_NEAR(loaded_transform.scale.y, transform.scale.y, 0.0001f);
	EXPECT_NEAR(loaded_transform.scale.z, transform.scale.z, 0.0001f);
}

// S_LocalBounds is deliberately not registered with C_ComponentRegistry (see the comment
// on its definition in CoreComponents.h) - it's a derived cache the render systems rebuild every
// frame, not authored data. Pins down that documented intent as a test rather than leaving it as
// a comment nobody would notice breaking.
TEST(WorldSerializer, SaveThenLoad_DoesNotPersistLocalBounds)
{
	C_World world;
	auto	entity = world.CreateEntity("Bounded");
	auto&	bounds = entity.Add<S_LocalBounds>();
	bounds.aabb.Add(glm::vec3(-1.f));
	bounds.aabb.Add(glm::vec3(1.f));
	const GUID originalGuid = entity.GetGuid();

	C_WorldSerializer  serializer;
	pugi::xml_document doc = serializer.Save(world);

	C_World			   loaded;
	Core::LoadingQuery query;
	serializer.Load(doc, loaded, Core::C_ResourceManager::Instance(), query, /*loadHandlesInstantly=*/true);

	C_Entity found = loaded.FindByGuid(originalGuid);
	ASSERT_TRUE(found.IsValid());
	EXPECT_FALSE(found.Has<S_LocalBounds>());
}

TEST(WorldSerializer, SaveThenLoad_PreservesHierarchy)
{
	C_World world;
	auto	parent = world.CreateEntity("Parent");
	auto	child  = world.CreateEntity("Child");
	ASSERT_TRUE(world.SetParent(child.Handle(), parent.Handle()));
	const GUID parentGuid = parent.GetGuid();
	const GUID childGuid  = child.GetGuid();

	C_WorldSerializer  serializer;
	pugi::xml_document doc = serializer.Save(world);

	C_World			   loaded;
	Core::LoadingQuery query;
	serializer.Load(doc, loaded, Core::C_ResourceManager::Instance(), query, true);

	C_Entity loadedChild  = loaded.FindByGuid(childGuid);
	C_Entity loadedParent = loaded.FindByGuid(parentGuid);
	ASSERT_TRUE(loadedChild.IsValid());
	ASSERT_TRUE(loadedParent.IsValid());
	const auto& rel = loaded.Registry().get<S_RelationshipComponent>(loadedChild.Handle());
	EXPECT_EQ(rel.parent, loadedParent.Handle());
}

TEST(WorldSerializer, Load_UnknownComponentName_SkippedWithoutCrashing)
{
	pugi::xml_document doc;
	auto			   worldNode		= doc.append_child("World");
	auto			   entityNode		= worldNode.append_child("Entity");
	entityNode.append_attribute("guid") = GUID("00000000-0000-0000-0000-000000000001").toString().c_str();
	entityNode.append_attribute("name") = "Weird";
	entityNode.append_child("ThisComponentDoesNotExist");

	C_WorldSerializer  serializer;
	C_World			   loaded;
	Core::LoadingQuery query;
	EXPECT_NO_FATAL_FAILURE(serializer.Load(doc, loaded, Core::C_ResourceManager::Instance(), query, true));
	EXPECT_TRUE(loaded.FindByName("Weird").IsValid());
}

TEST(WorldSerializer, Load_ParentGuidMissingFromFile_LeavesEntityUnparentedWithoutThrowing)
{
	pugi::xml_document doc;
	auto			   worldNode = doc.append_child("World");
	auto			   entityNode = worldNode.append_child("Entity");

	const GUID childGuid				   = GUID("00000000-0000-0000-0000-000000000001");
	entityNode.append_attribute("guid")   = childGuid.toString().c_str();
	entityNode.append_attribute("name")   = "Orphan";
	// Not the guid of any <Entity> in this file - e.g. a truncated save or a hand-edited one.
	entityNode.append_attribute("parent") = GUID("00000000-0000-0000-0000-00000000dead").toString().c_str();

	C_WorldSerializer  serializer;
	C_World			   loaded;
	Core::LoadingQuery query;
	EXPECT_NO_THROW(serializer.Load(doc, loaded, Core::C_ResourceManager::Instance(), query, true));

	C_Entity child = loaded.FindByGuid(childGuid);
	ASSERT_TRUE(child.IsValid());
	EXPECT_EQ(loaded.Registry().get<S_RelationshipComponent>(child.Handle()).parent, entt::null);
}

TEST(WorldSerializer, Load_CyclicParentReferences_RejectsTheSecondAndLeavesItUnparented)
{
	pugi::xml_document doc;
	auto			   worldNode = doc.append_child("World");

	const GUID aGuid = GUID("00000000-0000-0000-0000-00000000000a");
	const GUID bGuid = GUID("00000000-0000-0000-0000-00000000000b");

	auto aNode							= worldNode.append_child("Entity");
	aNode.append_attribute("guid")		= aGuid.toString().c_str();
	aNode.append_attribute("name")		= "A";
	aNode.append_attribute("parent")	= bGuid.toString().c_str();

	auto bNode						 = worldNode.append_child("Entity");
	bNode.append_attribute("guid")	 = bGuid.toString().c_str();
	bNode.append_attribute("name")	 = "B";
	bNode.append_attribute("parent") = aGuid.toString().c_str();

	C_WorldSerializer  serializer;
	C_World			   loaded;
	Core::LoadingQuery query;
	EXPECT_NO_THROW(serializer.Load(doc, loaded, Core::C_ResourceManager::Instance(), query, true));

	C_Entity a = loaded.FindByGuid(aGuid);
	C_Entity b = loaded.FindByGuid(bGuid);
	ASSERT_TRUE(a.IsValid());
	ASSERT_TRUE(b.IsValid());

	// A is processed first and parents under B successfully (B has no parent yet, so it is not
	// a cycle at that point). When B is then processed and asked to parent under A, SetParent
	// must reject it as a cycle (A's parent chain now reaches B) and leave B unparented.
	EXPECT_EQ(loaded.Registry().get<S_RelationshipComponent>(a.Handle()).parent, b.Handle());
	EXPECT_EQ(loaded.Registry().get<S_RelationshipComponent>(b.Handle()).parent, entt::null);
}

TEST(WorldSerializer, Load_DuplicateGuid_KeepsTheFirstAndSkipsTheSecondWithoutCrashing)
{
	pugi::xml_document doc;
	auto			   worldNode = doc.append_child("World");

	const GUID sharedGuid = GUID("00000000-0000-0000-0000-00000000dupe");

	auto firstNode					 = worldNode.append_child("Entity");
	firstNode.append_attribute("guid") = sharedGuid.toString().c_str();
	firstNode.append_attribute("name") = "First";

	auto secondNode					  = worldNode.append_child("Entity");
	secondNode.append_attribute("guid") = sharedGuid.toString().c_str();
	secondNode.append_attribute("name") = "Second";
	// A component child node is required to actually exercise the crash this fix prevents: without
	// one, the duplicate's invalid entity handle is never passed to entry->emplaceDefault().
	secondNode.append_child("Transform");

	C_WorldSerializer  serializer;
	C_World			   loaded;
	Core::LoadingQuery query;
	// Before this fix, CreateEntityWithGuid's invalid return for the duplicate was never checked,
	// so the component-deserialization loop below would call entry->emplaceDefault on an
	// entt::null handle.
	EXPECT_NO_THROW(serializer.Load(doc, loaded, Core::C_ResourceManager::Instance(), query, true));

	EXPECT_TRUE(loaded.FindByName("First").IsValid());
	EXPECT_FALSE(loaded.FindByName("Second").IsValid());
	EXPECT_EQ(loaded.FindByGuid(sharedGuid).GetName(), "First");
}

} // namespace GLEngine::Entity
