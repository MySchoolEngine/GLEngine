#pragma once

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

class TriangleTest : public BaseShape {
public:
	TriangleTest() = default;

	float base	 = 0.0f;
	float height = 0.0f;
	float angle	 = 0.0f;

	RTTR_ENABLE(BaseShape)
};

class TriangleRawPtrTest : public BaseShape {
public:
	TriangleRawPtrTest() = default;

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
};

class PointerContainer {
public:
	PointerContainer() = default;
	BaseShape* basePtr;
	Circle*	   circlePtr;
};