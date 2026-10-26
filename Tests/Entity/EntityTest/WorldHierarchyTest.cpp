#include <EntityTestStdafx.h>

#include <Entity/Components/CoreComponents.h>
#include <Entity/Entity.h>
#include <Entity/Systems/TransformSystem.h>
#include <Entity/World.h>

#include <glm/gtc/matrix_transform.hpp>

namespace GLEngine::Entity {

namespace {
bool MatrixNear(const glm::mat4& a, const glm::mat4& b, float eps = 0.0001f)
{
	for (int col = 0; col < 4; ++col)
		for (int row = 0; row < 4; ++row)
			if (std::abs(a[col][row] - b[col][row]) > eps)
				return false;
	return true;
}
} // namespace

TEST(WorldHierarchy, SetParent_PropagatesWorldTransform)
{
	C_World world;
	auto	parent = world.CreateEntity("Parent");
	auto	child  = world.CreateEntity("Child");

	parent.Get<S_TransformComponent>().translation = glm::vec3(10.f, 0.f, 0.f);
	child.Get<S_TransformComponent>().translation  = glm::vec3(1.f, 0.f, 0.f);

	ASSERT_TRUE(world.SetParent(child.Handle(), parent.Handle()));

	TransformSystem::Update(world);

	const auto expected = glm::translate(glm::mat4(1.f), glm::vec3(11.f, 0.f, 0.f));
	EXPECT_TRUE(MatrixNear(child.Get<S_WorldTransformComponent>().world, expected));
}

TEST(WorldHierarchy, SetParent_RejectsCycle)
{
	C_World world;
	auto	a = world.CreateEntity("A");
	auto	b = world.CreateEntity("B");
	auto	c = world.CreateEntity("C");

	ASSERT_TRUE(world.SetParent(b.Handle(), a.Handle()));
	ASSERT_TRUE(world.SetParent(c.Handle(), b.Handle()));

	// a -> b -> c already; making a a child of c would close the loop.
	EXPECT_FALSE(world.SetParent(a.Handle(), c.Handle()));
}

TEST(WorldHierarchy, Detach_RemovesFromParentAndResetsWorldTransform)
{
	C_World world;
	auto	parent								   = world.CreateEntity("Parent");
	auto	child								   = world.CreateEntity("Child");
	parent.Get<S_TransformComponent>().translation = glm::vec3(5.f, 0.f, 0.f);
	ASSERT_TRUE(world.SetParent(child.Handle(), parent.Handle()));
	TransformSystem::Update(world);

	world.Detach(child.Handle());
	TransformSystem::Update(world);

	EXPECT_TRUE(MatrixNear(child.Get<S_WorldTransformComponent>().world, glm::mat4(1.f)));
}

TEST(WorldHierarchy, Detach_MiddleChild_KeepsSiblingListConsistent)
{
	C_World world;
	auto	parent = world.CreateEntity("Parent");
	auto	a	   = world.CreateEntity("A");
	auto	b	   = world.CreateEntity("B");
	auto	c	   = world.CreateEntity("C");
	ASSERT_TRUE(world.SetParent(a.Handle(), parent.Handle()));
	ASSERT_TRUE(world.SetParent(b.Handle(), parent.Handle()));
	ASSERT_TRUE(world.SetParent(c.Handle(), parent.Handle()));

	world.Detach(b.Handle());

	const auto& aRel	  = world.Registry().get<S_RelationshipComponent>(a.Handle());
	const auto& cRel	  = world.Registry().get<S_RelationshipComponent>(c.Handle());
	const auto& parentRel = world.Registry().get<S_RelationshipComponent>(parent.Handle());
	EXPECT_EQ(parentRel.childCount, 2u);
	// Insertion order is head-first (SetParent inserts at firstChild), so after
	// inserting a, b, c in that order the list is c -> b -> a; removing b leaves c -> a.
	EXPECT_EQ(cRel.next, a.Handle());
	EXPECT_EQ(aRel.prev, c.Handle());
}

TEST(WorldHierarchy, DestroyEntity_ParentDeletesChildren)
{
	C_World	   world;
	auto	   parent	  = world.CreateEntity("Parent");
	auto	   a		  = world.CreateEntity("A");
	auto	   b		  = world.CreateEntity("B");
	auto	   c		  = world.CreateEntity("C");
	auto	   grandchild = world.CreateEntity("Grandchild");
	ASSERT_TRUE(world.SetParent(a.Handle(), parent.Handle()));
	ASSERT_TRUE(world.SetParent(b.Handle(), parent.Handle()));
	ASSERT_TRUE(world.SetParent(c.Handle(), parent.Handle()));
	ASSERT_TRUE(world.SetParent(grandchild.Handle(), a.Handle())); // nested one level deeper than a, b, c

	const GUID parentGuid	   = parent.GetGuid();
	const GUID aGuid		   = a.GetGuid();
	const GUID bGuid		   = b.GetGuid();
	const GUID cGuid		   = c.GetGuid();
	const GUID grandchildGuid = grandchild.GetGuid();

	ASSERT_TRUE(parent.IsValid());
	world.DestroyEntity(parent.Handle());

	// Destruction is deferred - the GUID lookup must still resolve until OnUpdate() flushes.
	EXPECT_TRUE(world.FindByGuid(parentGuid).IsValid());
	EXPECT_TRUE(world.FindByGuid(aGuid).IsValid());
	EXPECT_TRUE(world.FindByGuid(bGuid).IsValid());
	EXPECT_TRUE(world.FindByGuid(cGuid).IsValid());
	EXPECT_TRUE(world.FindByGuid(grandchildGuid).IsValid());

	world.OnUpdate();

	EXPECT_FALSE(world.FindByGuid(parentGuid).IsValid());
	EXPECT_FALSE(world.FindByGuid(aGuid).IsValid());
	EXPECT_FALSE(world.FindByGuid(bGuid).IsValid());
	EXPECT_FALSE(world.FindByGuid(cGuid).IsValid());
	EXPECT_FALSE(world.FindByGuid(grandchildGuid).IsValid());

	EXPECT_FALSE(world.FindByName("Parent").IsValid());
	EXPECT_FALSE(world.FindByName("A").IsValid());
	EXPECT_FALSE(world.FindByName("B").IsValid());
	EXPECT_FALSE(world.FindByName("C").IsValid());
	EXPECT_FALSE(world.FindByName("Grandchild").IsValid());

	// F-026 regression: a, b, c, and grandchild were marked dirty by SetParent above and never
	// re-propagated before the destroy. Before the fix, OnUpdate() destroyed only `parent` and
	// left them pointing at a dead/recycled parent handle, so this call dereferenced it.
	TransformSystem::Update(world);
}

} // namespace GLEngine::Entity
