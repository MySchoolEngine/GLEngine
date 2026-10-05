#include <UtilsTestStdafx.h>

#include <Utils/Reflection/Metadata.h>
#include <Utils/Serialization/XMLDeserialize.h>

#include <CommonTestUtils/XMLDeserializeFixture.h>

#include <UtilsTest/Serialization/DummyStructs.h>

namespace GLEngine::Utils {

// These tests document the intent that values built up during XML
// deserialization end up in their final location via a move rather than a
// copy - or, better still, via no reconstruction at all when an in-place
// reference already reaches the final location directly. All four now pass;
// history of what it took is below, in case any of it regresses.
//
// The Tracked* types in DummyStructs.h/.cpp are registered with
// rttr::policy::prop::as_reference_wrapper (same pattern as RefWrapperStruct),
// which lets DeserializeProperty mutate the owner's actual member in place
// instead of round-tripping through prop.get_value()/prop.set_value() on an
// independent copy. That removes one whole-object copy per test.
//
// What was left was a hard floor in the vendored RTTR (vendor/RTTR): both
// rttr::variant_sequential_view::insert() and variant_associative_view::insert()
// bottom out in container inserters that take `const value_t&` with no
// move-insert overload -- so every element pushed into a std::vector<T>/std::map<K,V>
// through these views was always copy-constructed, regardless of what value
// category was passed in from here.
//
// For the associative case this is now fixed in vendor/RTTR: associative_mapper_impl.h
// gained a value_t&& overload of insert_key_value (map and multimap specializations),
// a matching insert_key_value_move dispatcher, and variant_associative_view/
// variant_associative_view_private gained a new insert_move() entry point that routes
// through it. DeserializeAssociativeArray now calls view.insert_move(keyVar, valueVar)
// instead of view.insert(...), so AssociativeContainerValueIsMovedNotCopied passes.
//
// SequentialContainerElementIsMovedNotCopied hit the equivalent floor in
// sequential_mapper_impl.h. Only sequential_container_base_dynamic (backing
// std::vector/std::list/std::deque) actually needed the fix -- std::array, raw
// arrays, std::initializer_list and the fixed-size specializations already have
// no-op insert() implementations, and std::vector<bool>'s value_t is bool, where
// move vs. copy makes no difference. It gained the same shape of fix as the
// associative case: a value_t&& overload of insert(), an insert_move dispatcher,
// and variant_sequential_view/variant_sequential_view_private gained insert_move().
// DeserializeArray's non-atomic branch now calls view.insert_move(view.end(), var)
// instead of view.insert(...) -- the atomic branch still uses view.insert(...)
// deliberately, since it reads `var` again afterwards via view.set_value(index, var).
//
// NestedStructPropertyIsMovedNotCopied is now fixed too, and better than the name
// implies: DeserializeProperty's reference-wrapper branch (var aliases owner's own
// member) no longer calls prop.set_value(owner, varNode) at all afterwards - that
// call was a redundant self-assignment (varNode's reference_wrapper aliases the same
// object owner already holds), tracked via a new aliasesOwnerMember flag. Separately,
// the atomic-property branch's owner.get_type().is_wrapper() special case used to call
// owner.extract_wrapped_value() first, which - for a reference_wrapper owner - copy-
// constructs a throwaway TrackedValue snapshot only to have its value immediately
// discarded by DeserializeAtomic(); prop.get_value(owner) already resolves a wrapped
// owner via instance::try_convert()'s wrapped-type fallback (proved by the identical,
// already-unconditional propValue = prop.get_value(owner) a few lines above it in the
// same function), so that extraction was simply removed.
//
// With both of those gone, TrackedValue::m_Payload is mutated in place through the
// reference the whole way down: nothing about TrackedValue itself is ever
// constructed, copied, or moved - only the plain std::string member is assigned to.
// That is a strictly better outcome than "moved not copied", so unlike the two
// container tests above (which still expect >= N moves), this one now asserts zero
// TrackedValue-level operations altogether.
TEST_F(XMLDeserializeFixture, NestedStructPropertyIsMovedNotCopied)
{
	TrackedValue::s_Counters.Reset();

	const pugi::xml_document doc = ConstructDocument(R"x(
<?xml version="1.0"?>
<TrackedStruct>
	<tracked payload="hello" />
</TrackedStruct>)x");

	const auto result = deserializer.Deserialize<std::shared_ptr<TrackedStruct>>(doc);
	ASSERT_TRUE(result.has_value());
	EXPECT_EQ(result.value()->m_Tracked.m_Payload, "hello");

	EXPECT_EQ(TrackedValue::s_Counters.copyConstructions, 0);
	EXPECT_EQ(TrackedValue::s_Counters.copyAssignments, 0);
	EXPECT_EQ(TrackedValue::s_Counters.moveConstructions, 0);
	EXPECT_EQ(TrackedValue::s_Counters.moveAssignments, 0);
}

TEST_F(XMLDeserializeFixture, SequentialContainerElementIsMovedNotCopied)
{
	TrackedValue::s_Counters.Reset();

	const pugi::xml_document doc = ConstructDocument(R"x(
<?xml version="1.0"?>
<TrackedVectorStruct>
	<trackedVector>
		<TrackedValue payload="a" />
		<TrackedValue payload="b" />
	</trackedVector>
</TrackedVectorStruct>)x");

	const auto result = deserializer.Deserialize<std::shared_ptr<TrackedVectorStruct>>(doc);
	ASSERT_TRUE(result.has_value());
	ASSERT_EQ(result.value()->m_TrackedVector.size(), 2);
	EXPECT_EQ(result.value()->m_TrackedVector[0].m_Payload, "a");
	EXPECT_EQ(result.value()->m_TrackedVector[1].m_Payload, "b");

	EXPECT_EQ(TrackedValue::s_Counters.copyConstructions, 0);
	EXPECT_EQ(TrackedValue::s_Counters.copyAssignments, 0);
	EXPECT_GE(TrackedValue::s_Counters.moveConstructions + TrackedValue::s_Counters.moveAssignments, 2);
}

TEST_F(XMLDeserializeFixture, AssociativeContainerValueIsMovedNotCopied)
{
	TrackedValue::s_Counters.Reset();

	const pugi::xml_document doc = ConstructDocument(R"x(
<?xml version="1.0"?>
<TrackedMapStruct>
	<trackedMap>
		<item>
			<key value="k1" />
			<value>
				<TrackedValue payload="v1" />
			</value>
		</item>
	</trackedMap>
</TrackedMapStruct>)x");

	const auto result = deserializer.Deserialize<std::shared_ptr<TrackedMapStruct>>(doc);
	ASSERT_TRUE(result.has_value());
	ASSERT_EQ(result.value()->m_TrackedMap.size(), 1);
	EXPECT_EQ(result.value()->m_TrackedMap.at("k1").m_Payload, "v1");

	EXPECT_EQ(TrackedValue::s_Counters.copyConstructions, 0);
	EXPECT_EQ(TrackedValue::s_Counters.copyAssignments, 0);
	EXPECT_GE(TrackedValue::s_Counters.moveConstructions + TrackedValue::s_Counters.moveAssignments, 1);
}

// This is now fixed too. Converting the deserializer's internal shared_ptr<T>-wrapped
// result to a plain-by-value T used to fail for every registered type (verified
// against GlmVec3, not just TrackedStruct): type.create() commonly returns a
// std::shared_ptr<T> (RTTR's default constructor policy is as_std_shared_ptr), and
// there was no registered "unwrap to T by value" converter for variant::convert<T>()
// to fall back on - nor should every deserializable type need to register one just to
// support this pattern. C_XMLDeserializer::Deserialize<T>() (XMLDeserialize.h) now
// falls back to var.convert<std::shared_ptr<T>>() when convert<T>() isn't possible,
// and moves the pointee out of that shared_ptr into the returned std::optional<T>,
// since T is known statically there regardless of what registered converters exist.
TEST_F(XMLDeserializeFixture, TopLevelResultIsMovedNotCopiedOutOfDeserializer)
{
	TrackedValue::s_Counters.Reset();

	const pugi::xml_document doc = ConstructDocument(R"x(
<?xml version="1.0"?>
<TrackedStruct>
	<tracked payload="hello" />
</TrackedStruct>)x");

	const auto result = deserializer.Deserialize<TrackedStruct>(doc);
	ASSERT_TRUE(result.has_value());
	EXPECT_EQ(result->m_Tracked.m_Payload, "hello");

	EXPECT_EQ(TrackedValue::s_Counters.copyConstructions, 0);
	EXPECT_EQ(TrackedValue::s_Counters.copyAssignments, 0);
	EXPECT_GE(TrackedValue::s_Counters.moveConstructions + TrackedValue::s_Counters.moveAssignments, 1);
}

} // namespace GLEngine::Utils
