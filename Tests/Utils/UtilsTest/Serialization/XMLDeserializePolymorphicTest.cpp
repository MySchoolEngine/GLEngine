#include <UtilsTestStdafx.h>
#include <gtest/gtest.h>

#include <Utils/Serialization/XMLDeserialize.h>

#include <CommonTestUtils/XMLDeserializeFixture.h>

#include <UtilsTest/Serialization/DummyStructs_Polymorphic.h>

#include <pugixml.hpp>

namespace GLEngine::Utils {

TEST_F(XMLDeserializeFixture, DeserializeCircle)
{
	const pugi::xml_document doc = ConstructDocument(R"x(
<?xml version="1.0"?>
<ShapeContainer containerName="CircleContainer">
	<shape derivedTypeCast="Circle" id="1" name="TestCircle" radius="5.5" />
</ShapeContainer>)x");

	const auto result = deserializer.Deserialize<std::shared_ptr<ShapeContainer>>(doc);
	ASSERT_TRUE(result.has_value());

	const auto& container = result.value();
	EXPECT_EQ(container->containerName, "CircleContainer");

	ASSERT_TRUE(container->shape);
	EXPECT_EQ(container->shape->id, 1);
	EXPECT_EQ(container->shape->name, "TestCircle");

	const auto circle = std::dynamic_pointer_cast<Circle>(container->shape);
	ASSERT_TRUE(circle);
	EXPECT_FLOAT_EQ(circle->radius, 5.5f);
}

TEST_F(XMLDeserializeFixture, DeserializeRectangle)
{
	const pugi::xml_document doc = ConstructDocument(R"x(
<?xml version="1.0"?>
<ShapeContainer containerName="RectangleContainer">
	<shape derivedTypeCast="RectangleTest" id="2" name="TestRectangle" width="10.0" height="8.0" />
</ShapeContainer>)x");

	const auto result = deserializer.Deserialize<std::shared_ptr<ShapeContainer>>(doc);
	ASSERT_TRUE(result.has_value());

	const auto& container = result.value();
	EXPECT_EQ(container->containerName, "RectangleContainer");

	ASSERT_TRUE(container->shape);
	EXPECT_EQ(container->shape->id, 2);
	EXPECT_EQ(container->shape->name, "TestRectangle");

	const auto rectangle = std::dynamic_pointer_cast<RectangleTest>(container->shape);
	ASSERT_TRUE(rectangle);
	EXPECT_FLOAT_EQ(rectangle->width, 10.0f);
	EXPECT_FLOAT_EQ(rectangle->height, 8.0f);
}

TEST_F(XMLDeserializeFixture, DeserializeTriangle)
{
	const pugi::xml_document doc = ConstructDocument(R"x(
<?xml version="1.0"?>
<ShapeContainer containerName="TriangleContainer">
	<shape derivedTypeCast="TriangleTest" id="3" name="TestTriangle" base="6.0" height="4.0" angle="45.0" />
</ShapeContainer>)x");

	const auto result = deserializer.Deserialize<std::shared_ptr<ShapeContainer>>(doc);
	ASSERT_TRUE(result.has_value());

	const auto& container = result.value();
	EXPECT_EQ(container->containerName, "TriangleContainer");

	ASSERT_TRUE(container->shape);
	EXPECT_EQ(container->shape->id, 3);
	EXPECT_EQ(container->shape->name, "TestTriangle");

	const auto triangle = std::dynamic_pointer_cast<TriangleTest>(container->shape);
	ASSERT_TRUE(triangle);
	EXPECT_FLOAT_EQ(triangle->base, 6.0f);
	EXPECT_FLOAT_EQ(triangle->height, 4.0f);
	EXPECT_FLOAT_EQ(triangle->angle, 45.0f);
}

TEST_F(XMLDeserializeFixture, DeserializeMultipleShapes)
{
	const pugi::xml_document doc = ConstructDocument(R"x(
<?xml version="1.0"?>
<ShapeContainer containerName="MultiShapeContainer">
	<shape derivedTypeCast="Circle" id="1" name="Circle1" radius="3.0" />
	<shapes>
		<Circle id="1" name="Circle1" radius="3.0" />
		<RectangleTest id="2" name="Rectangle1" width="7.0" height="5.0" />
		<TriangleTest id="3" name="Triangle1" base="4.0" height="3.0" angle="60.0" />
	</shapes>
</ShapeContainer>)x");

	const auto result = deserializer.Deserialize<std::shared_ptr<ShapeContainer>>(doc);
	ASSERT_TRUE(result.has_value());

	const auto& container = result.value();
	EXPECT_EQ(container->containerName, "MultiShapeContainer");

	// Validate single shape property
	ASSERT_TRUE(container->shape);
	EXPECT_EQ(container->shape->id, 1);
	EXPECT_EQ(container->shape->name, "Circle1");
	const auto singleCircle = std::dynamic_pointer_cast<Circle>(container->shape);
	ASSERT_TRUE(singleCircle);
	EXPECT_FLOAT_EQ(singleCircle->radius, 3.0f);

	// Validate shapes vector
	ASSERT_EQ(container->shapes.size(), 3u);

	const auto circle = std::dynamic_pointer_cast<Circle>(container->shapes[0]);
	ASSERT_TRUE(circle);
	EXPECT_EQ(circle->id, 1);
	EXPECT_EQ(circle->name, "Circle1");
	EXPECT_FLOAT_EQ(circle->radius, 3.0f);

	const auto rectangle = std::dynamic_pointer_cast<RectangleTest>(container->shapes[1]);
	ASSERT_TRUE(rectangle);
	EXPECT_EQ(rectangle->id, 2);
	EXPECT_EQ(rectangle->name, "Rectangle1");
	EXPECT_FLOAT_EQ(rectangle->width, 7.0f);
	EXPECT_FLOAT_EQ(rectangle->height, 5.0f);

	const auto triangle = std::dynamic_pointer_cast<TriangleTest>(container->shapes[2]);
	ASSERT_TRUE(triangle);
	EXPECT_EQ(triangle->id, 3);
	EXPECT_EQ(triangle->name, "Triangle1");
	EXPECT_FLOAT_EQ(triangle->base, 4.0f);
	EXPECT_FLOAT_EQ(triangle->height, 3.0f);
	EXPECT_FLOAT_EQ(triangle->angle, 60.0f);
}

TEST_F(XMLDeserializeFixture, DeserializePointerProperty)
{
	const pugi::xml_document doc = ConstructDocument(R"x(
<?xml version="1.0"?>
<PointerContainer>
	<basePtr derivedTypeCast="TriangleRawPtrTest" id="1" name="TestTriangle" base="6.0" height="4.0" angle="45.0" />
</PointerContainer>)x");

	const auto result = deserializer.Deserialize<std::shared_ptr<PointerContainer>>(doc);
	ASSERT_TRUE(result.has_value());

	const auto& container = result.value();

	ASSERT_TRUE(container->basePtr);
	const auto* triangle = dynamic_cast<TriangleRawPtrTest*>(container->basePtr);
	ASSERT_TRUE(triangle);
	EXPECT_EQ(triangle->id, 1);
	EXPECT_EQ(triangle->name, "TestTriangle");
	EXPECT_FLOAT_EQ(triangle->base, 6.f);
	EXPECT_FLOAT_EQ(triangle->height, 4.f);
	EXPECT_FLOAT_EQ(triangle->angle, 45.f);

	delete container->basePtr;
}

} // namespace GLEngine::Utils
