#include <UtilsTestStdafx.h>

#include <Utils/Serialization/XMLDeserialize.h>
#include <Utils/Serialization/XMLSerialize.h>

#include <Core/Resources/LoadingQuery.h>
#include <Core/Resources/ResourceManager.h>

#include <UtilsTest/Serialization/DummyStructs.h>

namespace GLEngine::Utils {

// round-trips every type that shares GLMRTTI.cpp's vec2/vec3/vec4/quat parsing path in a
// single test, with non-trivial (non-zero, non-identity) values so an axis swap or a w/x
// ordering bug would move a value to the wrong field rather than passing by coincidence.
TEST(GlmTypesRoundTrip, SerializeThenDeserialize_PreservesEveryComponent)
{
	GlmTypesStruct original;
	original.m_Vec2 = glm::vec2(1.5f, -2.5f);
	original.m_Vec3 = glm::vec3(3.25f, -4.75f, 5.125f);
	original.m_Vec4 = glm::vec4(-1.f, 2.f, -3.f, 4.f);
	// Deliberately not a unit quaternion - this is a storage round-trip, and normalizing would
	// mask a component-order bug behind "still happens to be a valid rotation".
	original.m_Quat = glm::quat(0.9f, 0.1f, 0.2f, 0.3f); // glm::quat(w, x, y, z)

	C_XMLSerializer			 serializer;
	const pugi::xml_document doc = serializer.Serialize(original);

	Core::LoadingQuery query;
	C_XMLDeserializer  deserializer(Core::C_ResourceManager::Instance(), query, true);
	const auto		   result = deserializer.Deserialize<std::shared_ptr<GlmTypesStruct>>(doc);
	ASSERT_TRUE(result.has_value());

	const auto& loaded = *result.value();
	EXPECT_FLOAT_EQ(loaded.m_Vec2.x, original.m_Vec2.x);
	EXPECT_FLOAT_EQ(loaded.m_Vec2.y, original.m_Vec2.y);

	EXPECT_FLOAT_EQ(loaded.m_Vec3.x, original.m_Vec3.x);
	EXPECT_FLOAT_EQ(loaded.m_Vec3.y, original.m_Vec3.y);
	EXPECT_FLOAT_EQ(loaded.m_Vec3.z, original.m_Vec3.z);

	EXPECT_FLOAT_EQ(loaded.m_Vec4.x, original.m_Vec4.x);
	EXPECT_FLOAT_EQ(loaded.m_Vec4.y, original.m_Vec4.y);
	EXPECT_FLOAT_EQ(loaded.m_Vec4.z, original.m_Vec4.z);
	EXPECT_FLOAT_EQ(loaded.m_Vec4.w, original.m_Vec4.w);

	EXPECT_FLOAT_EQ(loaded.m_Quat.x, original.m_Quat.x);
	EXPECT_FLOAT_EQ(loaded.m_Quat.y, original.m_Quat.y);
	EXPECT_FLOAT_EQ(loaded.m_Quat.z, original.m_Quat.z);
	EXPECT_FLOAT_EQ(loaded.m_Quat.w, original.m_Quat.w);
}

// regression: a malformed field (empty between commas, or non-numeric) used to let
// std::stof's exception escape all the way out of Deserialize(). It must fail cleanly instead.
TEST(GlmTypesRoundTrip, DeserializingAMalformedQuat_FailsCleanlyInsteadOfThrowing)
{
	pugi::xml_document doc;
	std::stringstream  ss;
	ss << R"x(<?xml version="1.0"?><GlmTypesStruct quat="(1,,0,0)" />)x";
	ASSERT_EQ(doc.load(ss).status, pugi::xml_parse_status::status_ok);

	Core::LoadingQuery query;
	C_XMLDeserializer  deserializer(Core::C_ResourceManager::Instance(), query, true);
	EXPECT_NO_THROW({ std::ignore = deserializer.Deserialize<std::shared_ptr<GlmTypesStruct>>(doc); });
}

} // namespace GLEngine::Utils
