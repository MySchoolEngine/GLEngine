#include <EntityTestStdafx.h>

#include <Entity/ComponentRegistry.h>
#include <Entity/Entity.h>
#include <Entity/World.h>

namespace GLEngine::Entity {

namespace {
struct S_RegistryTestComponent {
	int value = 7;
};

const bool g_RegisterTestComponent = [] {
	C_ComponentRegistry::Instance().Register<S_RegistryTestComponent>("RegistryTestComponent");
	return true;
}();
} // namespace

TEST(ComponentRegistry, FindByName_FindsRegisteredEntry)
{
	const auto* entry = C_ComponentRegistry::Instance().FindByName("RegistryTestComponent");
	ASSERT_NE(entry, nullptr);
	EXPECT_EQ(entry->name, "RegistryTestComponent");
}

TEST(ComponentRegistry, FindByName_UnknownName_ReturnsNull)
{
	EXPECT_EQ(C_ComponentRegistry::Instance().FindByName("DoesNotExist"), nullptr);
}

TEST(ComponentRegistry, EmplaceDefault_HasAndGet_RoundTrip)
{
	C_World		world;
	auto		entity = world.CreateEntity("Test");
	const auto* entry  = C_ComponentRegistry::Instance().FindByName("RegistryTestComponent");
	ASSERT_NE(entry, nullptr);

	EXPECT_FALSE(entry->has(world.Registry(), entity.Handle()));
	entry->emplaceDefault(world.Registry(), entity.Handle());
	EXPECT_TRUE(entry->has(world.Registry(), entity.Handle()));

	rttr::instance instance = entry->get(world.Registry(), entity.Handle());
	ASSERT_TRUE(instance.is_valid());
	EXPECT_EQ(instance.get_type(), rttr::type::get<S_RegistryTestComponent>());
}

TEST(ComponentRegistry, Remove_ComponentNoLongerPresent)
{
	C_World		world;
	auto		entity = world.CreateEntity("Test");
	const auto* entry  = C_ComponentRegistry::Instance().FindByName("RegistryTestComponent");

	entry->emplaceDefault(world.Registry(), entity.Handle());
	entry->remove(world.Registry(), entity.Handle());

	EXPECT_FALSE(entry->has(world.Registry(), entity.Handle()));
}

TEST(ComponentRegistry, TransformComponent_IsRegistered)
{
	const auto* entry = C_ComponentRegistry::Instance().FindByName("Transform");
	ASSERT_NE(entry, nullptr);
	EXPECT_TRUE(entry->serializable);
	EXPECT_FALSE(entry->userAddable); // every entity already has one; not user-addable/removable
}

} // namespace GLEngine::Entity
