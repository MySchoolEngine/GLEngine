#include <EntityTestStdafx.h>

#include <Entity/ComponentRegistry.h>
#include <Entity/Entity.h>
#include <Entity/World.h>

#include <algorithm>

namespace GLEngine::Entity {

namespace {
struct S_RegistryTestComponent {
	int value = 7;
};

[[maybe_unused]] const bool g_RegisterTestComponent = [] {
	C_ComponentRegistry::Instance().Register<S_RegistryTestComponent>("RegistryTestComponent");
	return true;
}();

bool g_RegistryTestComponentPatched = false;
void OnRegistryTestComponentPatched(entt::registry&, entt::entity)
{
	g_RegistryTestComponentPatched = true;
}
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

	// entry->get() returns a reference-wrapped rttr::variant (std::reference_wrapper<T>, aliasing
	// the live component - see ComponentRegistry.h). An rttr::instance's own get_type() is a
	// pointer-to-wrapper type, so get_raw_type() strips that pointer before is_wrapper() reports
	// true; get_wrapped_instance() then drills through to the real component - same pattern as
	// C_XMLSerializer::SerializeObject's is_wrapper()/get_wrapped_instance() handling.
	rttr::instance instance = entry->get(world.Registry(), entity.Handle());
	ASSERT_TRUE(instance.is_valid());
	ASSERT_TRUE(instance.get_type().get_raw_type().is_wrapper());
	EXPECT_EQ(instance.get_wrapped_instance().get_type().get_raw_type(), rttr::type::get<S_RegistryTestComponent>());
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

TEST(ComponentRegistry, Patch_FiresOnUpdateSignalForTheRegisteredType)
{
	C_World		world;
	auto		entity = world.CreateEntity("Test");
	const auto* entry	= C_ComponentRegistry::Instance().FindByName("RegistryTestComponent");
	ASSERT_NE(entry, nullptr);

	entry->emplaceDefault(world.Registry(), entity.Handle());

	g_RegistryTestComponentPatched = false;
	world.Registry().on_update<S_RegistryTestComponent>().connect<&OnRegistryTestComponentPatched>();

	entry->patch(world.Registry(), entity.Handle());
	EXPECT_TRUE(g_RegistryTestComponentPatched);

	world.Registry().on_update<S_RegistryTestComponent>().disconnect<&OnRegistryTestComponentPatched>();
}

TEST(ComponentRegistry, TransformComponent_IsRegistered)
{
	const auto* entry = C_ComponentRegistry::Instance().FindByName("Transform");
	ASSERT_NE(entry, nullptr);
	EXPECT_TRUE(entry->serializable);
	EXPECT_FALSE(entry->userAddable); // every entity already has one; not user-addable/removable
}

TEST(ComponentRegistry, FindByName_EmptyName_ReturnsNull)
{
	EXPECT_EQ(C_ComponentRegistry::Instance().FindByName(""), nullptr);
}

TEST(ComponentRegistry, Entries_ReturnsBothSerializableAndNonSerializableEntries)
{
	struct S_NonSerializableTestComponent {
		int value = 0;
	};

	auto& registry = C_ComponentRegistry::Instance();
	registry.Register<S_NonSerializableTestComponent>("NonSerializableTestComponent", /*serializable*/ false);

	const auto* serializableEntry = registry.FindByName("RegistryTestComponent");
	ASSERT_NE(serializableEntry, nullptr);
	EXPECT_TRUE(serializableEntry->serializable);

	const auto* nonSerializableEntry = registry.FindByName("NonSerializableTestComponent");
	ASSERT_NE(nonSerializableEntry, nullptr);
	EXPECT_FALSE(nonSerializableEntry->serializable);

	const auto& entries = registry.Entries();
	const auto	hasEntryNamed = [&entries](const char* name) {
		return std::any_of(entries.begin(), entries.end(), [name](const S_ComponentRegistryEntry& e) { return e.name == name; });
	};
	EXPECT_TRUE(hasEntryNamed("RegistryTestComponent"));
	EXPECT_TRUE(hasEntryNamed("NonSerializableTestComponent"));
}

} // namespace GLEngine::Entity
