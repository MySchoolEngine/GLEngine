#include <UtilsTestStdafx.h>

#include <UtilsTest/Serialization/DummyStructs_Polymorphic.h>

// clang-format off
RTTR_REGISTRATION
{
	rttr::registration::class_<BaseShape>("BaseShape")
		.property("id", &BaseShape::id)
		.property("name", &BaseShape::name);

	rttr::registration::class_<Circle>("Circle")
	.constructor<>()(rttr::policy::ctor::as_std_shared_ptr)
		.property("radius", &Circle::radius);

	rttr::registration::class_<RectangleTest>("RectangleTest")
		.constructor<>()(rttr::policy::ctor::as_std_shared_ptr)
		.property("width", &RectangleTest::width)
		.property("height", &RectangleTest::height);

	rttr::registration::class_<TriangleTest>("TriangleTest")
		.constructor<>()(rttr::policy::ctor::as_std_shared_ptr)
		.property("base", &TriangleTest::base)
		.property("height", &TriangleTest::height)
		.property("angle", &TriangleTest::angle);

	rttr::registration::class_<TriangleRawPtrTest>("TriangleRawPtrTest")
		.constructor<>()(rttr::policy::ctor::as_raw_ptr)
		.property("base", &TriangleRawPtrTest::base)
		.property("height", &TriangleRawPtrTest::height)
		.property("angle", &TriangleRawPtrTest::angle);

	rttr::registration::class_<ShapeContainer>("ShapeContainer")
		.constructor<>()
		.property("shape", &ShapeContainer::shape)
		.property("shapes", &ShapeContainer::shapes)
		.property("containerName", &ShapeContainer::containerName);

	rttr::registration::class_<PointerContainer>("PointerContainer")
		.constructor<>()
		.property("basePtr", &PointerContainer::basePtr)
		.property("circlePtr", &PointerContainer::circlePtr);
	
	rttr::type::register_wrapper_converter_for_base_classes<std::shared_ptr<BaseShape>>();
	rttr::type::register_wrapper_converter_for_base_classes<std::shared_ptr<RectangleTest>>();
	rttr::type::register_wrapper_converter_for_base_classes<std::shared_ptr<TriangleTest>>();
	rttr::type::register_wrapper_converter_for_base_classes<std::shared_ptr<TriangleRawPtrTest>>();
	rttr::type::register_wrapper_converter_for_base_classes<std::shared_ptr<Circle>>();
	rttr::type::register_wrapper_converter_for_base_classes<std::shared_ptr<ShapeContainer>>();
	
	rttr::type::register_converter_func([](std::shared_ptr<RectangleTest> ptr, bool& ok) -> std::shared_ptr<BaseShape> {
		ok = true;
		return std::static_pointer_cast<BaseShape>(ptr);
	});
	rttr::type::register_converter_func([](std::shared_ptr<TriangleTest> ptr, bool& ok) -> std::shared_ptr<BaseShape> {
		ok = true;
		return std::static_pointer_cast<BaseShape>(ptr);
	});
	rttr::type::register_converter_func([](std::shared_ptr<TriangleRawPtrTest> ptr, bool& ok) -> std::shared_ptr<BaseShape> {
		ok = true;
		return std::static_pointer_cast<BaseShape>(ptr);
	});
	rttr::type::register_converter_func([](std::shared_ptr<Circle> ptr, bool& ok) -> std::shared_ptr<BaseShape> {
		ok = true;
		return std::static_pointer_cast<BaseShape>(ptr);
	});
}
// clang-format on