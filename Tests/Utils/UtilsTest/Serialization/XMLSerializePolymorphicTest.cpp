#include <UtilsTestStdafx.h>
#include <gtest/gtest.h>

#include <Utils/Serialization/XMLDeserialize.h>
#include <Utils/Serialization/XMLSerialize.h>

#include <CommonTestUtils/XMLSerializeFixture.h>

#include <memory>

class BaseShape {
public:
	BaseShape()			 = default;
	virtual ~BaseShape() = default;

	int			id = 0;
	std::string name;

	RTTR_ENABLE()
};

class Circle : public BaseShape {
public:
	Circle() = default;

	float radius = 0.0f;

	RTTR_ENABLE(BaseShape)
};

class RectangleTest : public BaseShape {
public:
	RectangleTest() = default;

	float width	 = 0.0f;
	float height = 0.0f;

	RTTR_ENABLE(BaseShape)
};

class Triangle : public BaseShape {
public:
	Triangle() = default;

	float base	 = 0.0f;
	float height = 0.0f;
	float angle	 = 0.0f;

	RTTR_ENABLE(BaseShape)
};

class ShapeContainer {
public:
	ShapeContainer() = default;

	std::shared_ptr<BaseShape>				shape;
	std::vector<std::shared_ptr<BaseShape>> shapes;
	std::string								containerName;

	RTTR_ENABLE()
};

// clang-format off
RTTR_REGISTRATION
{
	rttr::registration::class_<BaseShape>("BaseShape")
		.property("id", &BaseShape::id)
		.property("name", &BaseShape::name);

	rttr::registration::class_<Circle>("Circle")
	.constructor<>()(rttr::policy::ctor::as_object)
		.property("radius", &Circle::radius);

	rttr::registration::class_<RectangleTest>("RectangleTest")
		.constructor<>()
		.property("width", &RectangleTest::width)
		.property("height", &RectangleTest::height);

	rttr::registration::class_<Triangle>("Triangle")
		.constructor<>()
		.property("base", &Triangle::base)
		.property("height", &Triangle::height)
		.property("angle", &Triangle::angle);

	rttr::registration::class_<ShapeContainer>("ShapeContainer")
		.constructor<>()
		.property("shape", &ShapeContainer::shape)
		.property("shapes", &ShapeContainer::shapes)
		.property("containerName", &ShapeContainer::containerName);
}
// clang-format on

namespace GLEngine::Utils {
TEST_F(XMLSerializeFixture, SerializeCircle)
{
	const auto circle = std::make_shared<Circle>();
	circle->id		  = 1;
	circle->name	  = "TestCircle";
	circle->radius	  = 5.5f;

	ShapeContainer container;
	container.containerName = "CircleContainer";
	container.shape			= circle;

	auto serializedXml = serializer.Serialize(container);
	ASSERT_FALSE(serializedXml.empty());

	// Validate XML structure
	auto root = serializedXml.child("ShapeContainer");
	EXPECT_TRUE(root);
	EXPECT_STREQ("CircleContainer", root.attribute("containerName").value());

	auto shapeNode = root.child("shape");
	EXPECT_TRUE(shapeNode);
	EXPECT_STREQ("Circle", shapeNode.attribute("derivedTypeCast").value()) << "Missing derived type for instantiation";
	EXPECT_EQ(1, shapeNode.attribute("id").as_int());
	EXPECT_STREQ("TestCircle", shapeNode.attribute("name").value());
	EXPECT_FLOAT_EQ(5.5f, shapeNode.attribute("radius").as_float());
}

TEST_F(XMLSerializeFixture, SerializeRectangle)
{
	const auto rectangle = std::make_shared<RectangleTest>();
	rectangle->id		 = 2;
	rectangle->name		 = "TestRectangle";
	rectangle->width	 = 10.0f;
	rectangle->height	 = 8.0f;

	ShapeContainer container;
	container.containerName = "RectangleContainer";
	container.shape			= rectangle;

	auto serializedXml = serializer.Serialize(container);
	ASSERT_FALSE(serializedXml.empty());

	// Validate XML structure
	auto root = serializedXml.child("ShapeContainer");
	EXPECT_TRUE(root);
	EXPECT_STREQ("RectangleContainer", root.attribute("containerName").value());

	auto shapeNode = root.child("shape");
	EXPECT_TRUE(shapeNode);
	EXPECT_STREQ("RectangleTest", shapeNode.attribute("derivedTypeCast").value()) << "Missing derived type for instantiation";
	EXPECT_EQ(2, shapeNode.attribute("id").as_int());
	EXPECT_STREQ("TestRectangle", shapeNode.attribute("name").value());
	EXPECT_FLOAT_EQ(10.0f, shapeNode.attribute("width").as_float());
	EXPECT_FLOAT_EQ(8.0f, shapeNode.attribute("height").as_float());
}

TEST_F(XMLSerializeFixture, SerializeTriangle)
{
	const auto triangle = std::make_shared<Triangle>();
	triangle->id		= 3;
	triangle->name		= "TestTriangle";
	triangle->base		= 6.0f;
	triangle->height	= 4.0f;
	triangle->angle		= 45.0f;

	ShapeContainer container;
	container.containerName = "TriangleContainer";
	container.shape			= triangle;

	auto serializedXml = serializer.Serialize(container);
	ASSERT_FALSE(serializedXml.empty());

	// Validate XML structure
	auto root = serializedXml.child("ShapeContainer");
	EXPECT_TRUE(root);
	EXPECT_STREQ("TriangleContainer", root.attribute("containerName").value());

	auto shapeNode = root.child("shape");
	EXPECT_TRUE(shapeNode);
	EXPECT_STREQ("Triangle", shapeNode.attribute("derivedTypeCast").value()) << "Missing derived type for instantiation";
	EXPECT_EQ(3, shapeNode.attribute("id").as_int());
	EXPECT_STREQ("TestTriangle", shapeNode.attribute("name").value());
	EXPECT_FLOAT_EQ(6.0f, shapeNode.attribute("base").as_float());
	EXPECT_FLOAT_EQ(4.0f, shapeNode.attribute("height").as_float());
	EXPECT_FLOAT_EQ(45.0f, shapeNode.attribute("angle").as_float());
}

TEST_F(XMLSerializeFixture, SerializeMultipleShapes)
{
	ShapeContainer container;
	container.containerName = "MultiShapeContainer";

	auto circle	   = std::make_shared<Circle>();
	circle->id	   = 1;
	circle->name   = "Circle1";
	circle->radius = 3.0f;

	auto rectangle	  = std::make_shared<RectangleTest>();
	rectangle->id	  = 2;
	rectangle->name	  = "Rectangle1";
	rectangle->width  = 7.0f;
	rectangle->height = 5.0f;

	auto triangle	 = std::make_shared<Triangle>();
	triangle->id	 = 3;
	triangle->name	 = "Triangle1";
	triangle->base	 = 4.0f;
	triangle->height = 3.0f;
	triangle->angle	 = 60.0f;

	container.shapes = {circle, rectangle, triangle};
	container.shape	 = circle;

	auto serializedXml = serializer.Serialize(container);
	ASSERT_FALSE(serializedXml.empty());

	// Validate XML structure
	auto root = serializedXml.child("ShapeContainer");
	EXPECT_TRUE(root);
	EXPECT_STREQ("MultiShapeContainer", root.attribute("containerName").value());

	// Validate single shape property
	auto shapeNode = root.child("shape");
	EXPECT_TRUE(shapeNode);
	EXPECT_STREQ("Circle", shapeNode.attribute("derivedTypeCast").value()) << "Missing derived type for instantiation";
	EXPECT_EQ(1, shapeNode.attribute("id").as_int());
	EXPECT_STREQ("Circle1", shapeNode.attribute("name").value());
	EXPECT_FLOAT_EQ(3.0f, shapeNode.attribute("radius").as_float());

	// Validate shapes vector
	auto shapesNode = root.child("shapes");
	EXPECT_TRUE(shapesNode);

	// Count and validate each shape in the vector
	int shapeCount = 0;
	for (auto child : shapesNode.children())
	{
		shapeCount++;
		EXPECT_TRUE(child);

		auto derivedType = child.name();
		auto id			 = child.attribute("id").as_int();

		if (strcmp(derivedType, "Circle") == 0)
		{
			EXPECT_EQ(1, id);
			EXPECT_STREQ("Circle1", child.attribute("name").value());
			EXPECT_FLOAT_EQ(3.0f, child.attribute("radius").as_float());
		}
		else if (strcmp(derivedType, "RectangleTest") == 0)
		{
			EXPECT_EQ(2, id);
			EXPECT_STREQ("Rectangle1", child.attribute("name").value());
			EXPECT_FLOAT_EQ(7.0f, child.attribute("width").as_float());
			EXPECT_FLOAT_EQ(5.0f, child.attribute("height").as_float());
		}
		else if (strcmp(derivedType, "Triangle") == 0)
		{
			EXPECT_EQ(3, id);
			EXPECT_STREQ("Triangle1", child.attribute("name").value());
			EXPECT_FLOAT_EQ(4.0f, child.attribute("base").as_float());
			EXPECT_FLOAT_EQ(3.0f, child.attribute("height").as_float());
			EXPECT_FLOAT_EQ(60.0f, child.attribute("angle").as_float());
		}
		else
		{
			FAIL() << "Unexpected derived type: " << derivedType;
		}
	}

	EXPECT_EQ(3, shapeCount) << "Expected 3 shapes in the vector";
}
} // namespace GLEngine::Utils