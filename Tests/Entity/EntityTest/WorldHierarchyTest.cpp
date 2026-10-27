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

TEST(WorldHierarchy, SetParent_ParentScaleAffectsChildTranslation)
{
	C_World world;
	auto	parent = world.CreateEntity("Parent");
	auto	child  = world.CreateEntity("Child");

	parent.Get<S_TransformComponent>().scale	   = glm::vec3(2.f, 2.f, 2.f);
	child.Get<S_TransformComponent>().translation = glm::vec3(1.f, 0.f, 0.f);

	ASSERT_TRUE(world.SetParent(child.Handle(), parent.Handle()));

	TransformSystem::Update(world);

	// A commuting bug (e.g. applying translation before scale, or ignoring scale in ComputeLocal)
	// would still land the child at (1,0,0) instead of (2,0,0) - scale and translation do not
	// commute, unlike the translation-only case above.
	const auto expected = glm::scale(glm::mat4(1.f), glm::vec3(2.f, 2.f, 2.f)) * glm::translate(glm::mat4(1.f), glm::vec3(1.f, 0.f, 0.f));
	EXPECT_TRUE(MatrixNear(child.Get<S_WorldTransformComponent>().world, expected));
	EXPECT_TRUE(MatrixNear(glm::translate(glm::mat4(1.f), glm::vec3(child.Get<S_WorldTransformComponent>().world[3])), glm::translate(glm::mat4(1.f), glm::vec3(2.f, 0.f, 0.f))));
}

TEST(WorldHierarchy, SetParent_PropagatesThroughThreeLevelChain)
{
	C_World world;
	auto	grandparent = world.CreateEntity("Grandparent");
	auto	parent		= world.CreateEntity("Parent");
	auto	child		= world.CreateEntity("Child");

	grandparent.Get<S_TransformComponent>().scale		 = glm::vec3(2.f, 2.f, 2.f);
	parent.Get<S_TransformComponent>().translation		 = glm::vec3(1.f, 0.f, 0.f);
	child.Get<S_TransformComponent>().translation		 = glm::vec3(0.f, 1.f, 0.f);

	ASSERT_TRUE(world.SetParent(parent.Handle(), grandparent.Handle()));
	ASSERT_TRUE(world.SetParent(child.Handle(), parent.Handle()));

	TransformSystem::Update(world);

	// Grandparent's scale must apply once, at the root, not re-applied at every level: parent's
	// local translation (1,0,0) is scaled by the grandparent to world (2,0,0), and the child's
	// local translation (0,1,0) is added in grandparent-scaled space on top of that, landing at
	// (2,2,0) - not (3,1,0) (ignoring scale entirely) and not (2,3,0) (scaling only the child's
	// own offset instead of the whole accumulated parent chain).
	const auto parentExpected = glm::vec3(2.f, 0.f, 0.f);
	const auto childExpected  = glm::vec3(2.f, 2.f, 0.f);
	EXPECT_TRUE(MatrixNear(glm::translate(glm::mat4(1.f), glm::vec3(parent.Get<S_WorldTransformComponent>().world[3])), glm::translate(glm::mat4(1.f), parentExpected)));
	EXPECT_TRUE(MatrixNear(glm::translate(glm::mat4(1.f), glm::vec3(child.Get<S_WorldTransformComponent>().world[3])), glm::translate(glm::mat4(1.f), childExpected)));
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

	// A rejected SetParent must leave the existing graph exactly as it was.
	const auto& aRel = world.Registry().get<S_RelationshipComponent>(a.Handle());
	const auto& cRel = world.Registry().get<S_RelationshipComponent>(c.Handle());
	EXPECT_EQ(aRel.parent, entt::null);
	EXPECT_EQ(cRel.parent, b.Handle());

	// Self-parenting is a degenerate one-node cycle and must also be rejected.
	EXPECT_FALSE(world.SetParent(a.Handle(), a.Handle()));
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

TEST(WorldHierarchy, Detach_HeadChild_UpdatesParentFirstChild)
{
	C_World world;
	auto	parent = world.CreateEntity("Parent");
	auto	a	   = world.CreateEntity("A");
	auto	b	   = world.CreateEntity("B");
	auto	c	   = world.CreateEntity("C");
	ASSERT_TRUE(world.SetParent(a.Handle(), parent.Handle()));
	ASSERT_TRUE(world.SetParent(b.Handle(), parent.Handle()));
	ASSERT_TRUE(world.SetParent(c.Handle(), parent.Handle()));

	// Insertion order is head-first, so the list is c -> b -> a; c is parent.firstChild - the
	// exact branch in World::Detach (parentRel.firstChild = childRel.next) this exercises.
	ASSERT_EQ(world.Registry().get<S_RelationshipComponent>(parent.Handle()).firstChild, c.Handle());

	world.Detach(c.Handle());

	const auto& parentRel = world.Registry().get<S_RelationshipComponent>(parent.Handle());
	const auto& bRel	   = world.Registry().get<S_RelationshipComponent>(b.Handle());
	EXPECT_EQ(parentRel.childCount, 2u);
	EXPECT_EQ(parentRel.firstChild, b.Handle()); // must move off the detached head
	EXPECT_EQ(bRel.prev, entt::null);			  // new head has no prev
}

TEST(WorldHierarchy, Detach_TailChild_LeavesNewTailWithNullNext)
{
	C_World world;
	auto	parent = world.CreateEntity("Parent");
	auto	a	   = world.CreateEntity("A");
	auto	b	   = world.CreateEntity("B");
	auto	c	   = world.CreateEntity("C");
	ASSERT_TRUE(world.SetParent(a.Handle(), parent.Handle()));
	ASSERT_TRUE(world.SetParent(b.Handle(), parent.Handle()));
	ASSERT_TRUE(world.SetParent(c.Handle(), parent.Handle()));

	// List is c -> b -> a; a is the tail (next == entt::null, the branch that is skipped rather
	// than updating a sibling's prev).
	ASSERT_EQ(world.Registry().get<S_RelationshipComponent>(a.Handle()).next, entt::null);

	world.Detach(a.Handle());

	const auto& parentRel = world.Registry().get<S_RelationshipComponent>(parent.Handle());
	const auto& bRel	   = world.Registry().get<S_RelationshipComponent>(b.Handle());
	EXPECT_EQ(parentRel.childCount, 2u);
	EXPECT_EQ(parentRel.firstChild, c.Handle()); // head unaffected by detaching the tail
	EXPECT_EQ(bRel.next, entt::null);			  // new tail has no next
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

	// a, b, c, and grandchild were marked dirty by SetParent above and never
	// re-propagated before the destroy. Before the fix, OnUpdate() destroyed only `parent` and
	// left them pointing at a dead/recycled parent handle, so this call dereferenced it.
	TransformSystem::Update(world);
}

TEST(WorldHierarchy, DestroyEntity_Child_ParentFirstChildAndSiblingsDoNotDangle)
{
	C_World world;
	auto	parent = world.CreateEntity("Parent");
	auto	a	   = world.CreateEntity("A");
	auto	b	   = world.CreateEntity("B");
	ASSERT_TRUE(world.SetParent(a.Handle(), parent.Handle()));
	ASSERT_TRUE(world.SetParent(b.Handle(), parent.Handle()));

	// Insertion order is head-first (SetParent inserts at firstChild), so after a, b the list is
	// b -> a: b is parent's firstChild, the exact link that would be left dangling if OnUpdate()
	// destroyed a child without first Detach()ing it from its parent.
	ASSERT_EQ(world.Registry().get<S_RelationshipComponent>(parent.Handle()).firstChild, b.Handle());

	const GUID bGuid = b.GetGuid();
	world.DestroyEntity(b.Handle());
	world.OnUpdate();

	EXPECT_FALSE(world.FindByGuid(bGuid).IsValid());

	const auto& parentRel = world.Registry().get<S_RelationshipComponent>(parent.Handle());
	EXPECT_EQ(parentRel.childCount, 1u);
	EXPECT_EQ(parentRel.firstChild, a.Handle()); // must not still point at the destroyed b

	const auto& aRel = world.Registry().get<S_RelationshipComponent>(a.Handle());
	EXPECT_EQ(aRel.parent, parent.Handle());
	EXPECT_EQ(aRel.prev, entt::null); // must not still point at the destroyed b

	// Regression guard: if firstChild (or a's prev) still referenced the destroyed b, walking
	// down from parent during the next propagation would dereference that dead handle.
	TransformSystem::Update(world);
}

} // namespace GLEngine::Entity
