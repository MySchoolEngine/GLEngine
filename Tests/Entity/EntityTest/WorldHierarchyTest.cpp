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

	parent.Get<S_TransformComponent>().scale	  = glm::vec3(2.f, 2.f, 2.f);
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

	grandparent.Get<S_TransformComponent>().scale  = glm::vec3(2.f, 2.f, 2.f);
	parent.Get<S_TransformComponent>().translation = glm::vec3(1.f, 0.f, 0.f);
	child.Get<S_TransformComponent>().translation  = glm::vec3(0.f, 1.f, 0.f);

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

TEST(WorldHierarchy, SetParent_StaleHandle_ReturnsFalseInsteadOfAsserting)
{
	C_World world;
	auto	stale = world.CreateEntity("Stale");
	world.DestroyEntity(stale.Handle());
	world.OnUpdate(); // handle is now invalid

	auto live = world.CreateEntity("Live");

	EXPECT_FALSE(world.SetParent(stale.Handle(), live.Handle()));
	EXPECT_FALSE(world.SetParent(live.Handle(), stale.Handle()));
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
	const auto& bRel	  = world.Registry().get<S_RelationshipComponent>(b.Handle());
	EXPECT_EQ(parentRel.childCount, 2u);
	EXPECT_EQ(parentRel.firstChild, b.Handle()); // must move off the detached head
	EXPECT_EQ(bRel.prev, entt::null);			 // new head has no prev
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
	const auto& bRel	  = world.Registry().get<S_RelationshipComponent>(b.Handle());
	EXPECT_EQ(parentRel.childCount, 2u);
	EXPECT_EQ(parentRel.firstChild, c.Handle()); // head unaffected by detaching the tail
	EXPECT_EQ(bRel.next, entt::null);			 // new tail has no next
}

TEST(WorldHierarchy, DestroyEntity_ParentDeletesChildren)
{
	C_World world;
	auto	parent	   = world.CreateEntity("Parent");
	auto	a		   = world.CreateEntity("A");
	auto	b		   = world.CreateEntity("B");
	auto	c		   = world.CreateEntity("C");
	auto	grandchild = world.CreateEntity("Grandchild");
	ASSERT_TRUE(world.SetParent(a.Handle(), parent.Handle()));
	ASSERT_TRUE(world.SetParent(b.Handle(), parent.Handle()));
	ASSERT_TRUE(world.SetParent(c.Handle(), parent.Handle()));
	ASSERT_TRUE(world.SetParent(grandchild.Handle(), a.Handle())); // nested one level deeper than a, b, c

	const GUID parentGuid	  = parent.GetGuid();
	const GUID aGuid		  = a.GetGuid();
	const GUID bGuid		  = b.GetGuid();
	const GUID cGuid		  = c.GetGuid();
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

TEST(WorldHierarchy, RegistryPatch_MarksTransformDirtyAndPropagatesToChildren)
{
	C_World world;
	auto	parent								  = world.CreateEntity("Parent");
	auto	child								  = world.CreateEntity("Child");
	child.Get<S_TransformComponent>().translation = glm::vec3(1.f, 0.f, 0.f);
	ASSERT_TRUE(world.SetParent(child.Handle(), parent.Handle()));

	// Establish a baseline and consume the dirty tags SetParent/CreateEntity already set, so the
	// propagation below can only be explained by the patch signal connected in C_World's
	// constructor (World.cpp: on_update<S_TransformComponent> -> S_DirtyTransformTag), not by
	// leftover dirty state from setup.
	TransformSystem::Update(world);
	ASSERT_TRUE(MatrixNear(child.Get<S_WorldTransformComponent>().world, glm::translate(glm::mat4(1.f), glm::vec3(1.f, 0.f, 0.f))));

	// Mutate in place through the component reference, then Patch() with no callback purely to
	// fire the on_update signal - the exact pattern the Editor's gizmo path uses
	// (EditorLayer.cpp).
	parent.Get<S_TransformComponent>().translation = glm::vec3(10.f, 0.f, 0.f);
	parent.Patch<S_TransformComponent>();

	TransformSystem::Update(world);

	const auto expectedParentWorld = glm::translate(glm::mat4(1.f), glm::vec3(10.f, 0.f, 0.f));
	const auto expectedChildWorld  = glm::translate(glm::mat4(1.f), glm::vec3(11.f, 0.f, 0.f));
	EXPECT_TRUE(MatrixNear(parent.Get<S_WorldTransformComponent>().world, expectedParentWorld));
	EXPECT_TRUE(MatrixNear(child.Get<S_WorldTransformComponent>().world, expectedChildWorld));
}

TEST(WorldHierarchy, Update_DeepChain_DoesNotOverflowTheStackAndPropagatesCorrectly)
{
	// F-016 fixed: each incremental Update() call here now does O(1) work (one dirty leaf, whose
	// parent is never also dirty) instead of walking to the root - this loop used to be O(kDepth^2)
	// on the Update() side alone, so don't casually re-introduce that without re-checking this
	// comment still holds.
	constexpr int kDepth = 500;

	C_World world;
	auto	root								 = world.CreateEntity("Node0");
	root.Get<S_TransformComponent>().translation = glm::vec3(1.f, 0.f, 0.f);
	TransformSystem::Update(world);

	auto previous = root;
	for (int i = 1; i < kDepth; ++i)
	{
		auto node									 = world.CreateEntity("Node" + std::to_string(i));
		node.Get<S_TransformComponent>().translation = glm::vec3(1.f, 0.f, 0.f);
		ASSERT_TRUE(world.SetParent(node.Handle(), previous.Handle()));
		TransformSystem::Update(world);
		previous = node;
	}
	const auto leaf = previous;

	const auto expected = glm::translate(glm::mat4(1.f), glm::vec3(static_cast<float>(kDepth), 0.f, 0.f));
	EXPECT_TRUE(MatrixNear(leaf.Get<S_WorldTransformComponent>().world, expected));
}

TEST(WorldHierarchy, Update_WholeChainDirtyInOneCall_PropagatesCorrectlyRegardlessOfDirtyIterationOrder)
{
	// F-016 regression: simulates level load, where every entity is tagged dirty by CreateEntity
	// itself and nothing calls Update() in between - registry.view<S_DirtyTransformTag>()'s
	// iteration order is unspecified and does not respect hierarchy depth, so a fix that seeds
	// its worklist from *every* dirty node (instead of just the roots of each dirty subtree)
	// could process a child before its parent's new world is ready, silently using a stale
	// parent world.
	C_World world;
	auto	grandparent = world.CreateEntity("Grandparent");
	auto	parent		= world.CreateEntity("Parent");
	auto	child		= world.CreateEntity("Child");

	grandparent.Get<S_TransformComponent>().translation = glm::vec3(10.f, 0.f, 0.f);
	parent.Get<S_TransformComponent>().translation		= glm::vec3(1.f, 0.f, 0.f);
	child.Get<S_TransformComponent>().translation		= glm::vec3(0.f, 1.f, 0.f);

	// SetParent itself re-tags the child dirty, but every entity is already dirty from
	// CreateEntity above - no TransformSystem::Update() call happens until after the whole chain
	// is built, so all three remain dirty simultaneously, same as a level load.
	ASSERT_TRUE(world.SetParent(parent.Handle(), grandparent.Handle()));
	ASSERT_TRUE(world.SetParent(child.Handle(), parent.Handle()));

	TransformSystem::Update(world);

	const auto expectedGrandparent = glm::translate(glm::mat4(1.f), glm::vec3(10.f, 0.f, 0.f));
	const auto expectedParent	   = glm::translate(glm::mat4(1.f), glm::vec3(11.f, 0.f, 0.f));
	const auto expectedChild	   = glm::translate(glm::mat4(1.f), glm::vec3(11.f, 1.f, 0.f));
	EXPECT_TRUE(MatrixNear(grandparent.Get<S_WorldTransformComponent>().world, expectedGrandparent));
	EXPECT_TRUE(MatrixNear(parent.Get<S_WorldTransformComponent>().world, expectedParent));
	EXPECT_TRUE(MatrixNear(child.Get<S_WorldTransformComponent>().world, expectedChild));
}

TEST(WorldHierarchy, DestroyEntity_ParentAndChildBothQueuedSameFrame_NoCrashNoDoubleDestroy)
{
	C_World world;
	auto	parent = world.CreateEntity("Parent");
	auto	child  = world.CreateEntity("Child");
	ASSERT_TRUE(world.SetParent(child.Handle(), parent.Handle()));

	const GUID parentGuid = parent.GetGuid();
	const GUID childGuid  = child.GetGuid();

	// Both independently queued in the same frame, parent first - the exact ordering
	// worried about: child's own queue entry runs Detach()/CollectSubtree() again on a subtree
	// already collected (and about to be destroyed) via parent's entry.
	world.DestroyEntity(parent.Handle());
	world.DestroyEntity(child.Handle());
	world.OnUpdate();

	EXPECT_FALSE(world.FindByGuid(parentGuid).IsValid());
	EXPECT_FALSE(world.FindByGuid(childGuid).IsValid());
}

} // namespace GLEngine::Entity
