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
	auto	entity								   = world.CreateEntity("floor");
	entity.Get<S_TransformComponent>().translation = glm::vec3(0.f, -1.5f, 0.f);
	const GUID originalGuid						   = entity.GetGuid();

	C_WorldSerializer  serializer;
	pugi::xml_document doc = serializer.Save(world);

	C_World			   loaded;
	Core::LoadingQuery query;
	serializer.Load(doc, loaded, Core::C_ResourceManager::Instance(), query, /*loadHandlesInstantly=*/true);

	C_Entity found = loaded.FindByGuid(originalGuid);
	ASSERT_TRUE(found.IsValid());
	EXPECT_EQ(found.GetName(), "floor");
	EXPECT_FLOAT_EQ(found.Get<S_TransformComponent>().translation.y, -1.5f);
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

} // namespace GLEngine::Entity
