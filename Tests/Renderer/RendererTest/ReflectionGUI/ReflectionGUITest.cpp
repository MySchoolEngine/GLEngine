#include <RendererTestStdafx.h>

#include <GUI/ReflectionGUI.h>

#include <Utils/Reflection/Metadata.h>

#include <rttr/registration>

namespace GLEngine::Renderer {

namespace {

struct S_CustomWidgetTestType {
	int value = 0;
};

struct S_ContainerOfCustomWidgets {
	std::vector<S_CustomWidgetTestType> items;
};

bool g_DrawFunctionCalled		= false;
bool g_DrawFunctionReturnValue = false;

bool TestDrawFunction(rttr::instance&, const rttr::property*)
{
	g_DrawFunctionCalled = true;
	return g_DrawFunctionReturnValue;
}

} // namespace

} // namespace GLEngine::Renderer

RTTR_REGISTRATION
{
	using namespace ::Utils::Reflection;
	// RegisterMetaclass<MetaGUI::CustomGUIWidget>() is the marker, attached here since this type
	// owns its own drawing (same module) - GUI::RegisterCustomWidgetDrawer carries the actual
	// function instead of RTTR metadata, since that's what a cross-module drawer (e.g. Editor
	// drawing a Renderer-owned resource type) is forced to use; this test exercises the same path
	// a same-module registration would.
	rttr::registration::class_<GLEngine::Renderer::S_CustomWidgetTestType>("S_CustomWidgetTestType")
	(
		RegisterMetaclass<MetaGUI::CustomGUIWidget>()
	)
	.constructor<>()(rttr::policy::ctor::as_object)
	.property("Value", &GLEngine::Renderer::S_CustomWidgetTestType::value);

	GLEngine::GUI::RegisterCustomWidgetDrawer(rttr::type::get<GLEngine::Renderer::S_CustomWidgetTestType>(), &GLEngine::Renderer::TestDrawFunction);

	rttr::registration::class_<GLEngine::Renderer::S_ContainerOfCustomWidgets>("S_ContainerOfCustomWidgets")
		.constructor<>()(rttr::policy::ctor::as_object)
		.property("Items", &GLEngine::Renderer::S_ContainerOfCustomWidgets::items)(rttr::policy::prop::as_reference_wrapper);
}

namespace GLEngine::Renderer {

TEST(ReflectionGUI, DrawAllPropertyGUI_CustomWidgetRegisteredOnWrappedInstance_IsFound)
{
	g_DrawFunctionCalled	   = false;
	g_DrawFunctionReturnValue = false;

	S_CustomWidgetTestType value;
	// std::reference_wrapper is the exact wrapper scenario the bug was about: get_type() on it
	// returns the wrapper's own type, not S_CustomWidgetTestType's, so the custom-widget metadata
	// lookup must unwrap first or it silently falls through to the generic property-drawing path.
	std::reference_wrapper<S_CustomWidgetTestType> wrapped(value);
	rttr::instance									 obj(wrapped);

	const auto changedProps = GUI::DrawAllPropertyGUI(obj);

	EXPECT_TRUE(g_DrawFunctionCalled);
	EXPECT_TRUE(changedProps.empty()); // DrawFunction reported no change, nothing to report back
}

TEST(ReflectionGUI, DrawAllPropertyGUI_CustomWidgetReportsAChange_ReturnsNonEmpty)
{
	g_DrawFunctionCalled	   = false;
	g_DrawFunctionReturnValue = true;

	S_CustomWidgetTestType value;
	std::reference_wrapper<S_CustomWidgetTestType> wrapped(value);
	rttr::instance									 obj(wrapped);

	const auto changedProps = GUI::DrawAllPropertyGUI(obj);

	EXPECT_TRUE(g_DrawFunctionCalled);
	// DrawFunction owns the whole instance, not a specific property, so a reported change is
	// surfaced as every property of the type rather than a precise changed-properties list.
	EXPECT_FALSE(changedProps.empty());
}

TEST(ReflectionGUI, CustomGUIWidgetTaggedType_FoundThroughSequentialContainerElement)
{
	// Regression test: an element obtained via create_sequential_view() unwraps to a POINTER
	// type (e.g. "S_CustomWidgetTestType*"), not the value type the metadata was registered on -
	// IsTypeUIMetaClass must strip that via get_raw_type() or this silently returns false, which
	// is exactly what broke every resource-picker widget inside an array property (e.g.
	// StaticMesh's "Materials" list) after the CustomGUIWidget-registry redesign.
	using namespace ::Utils::Reflection;

	S_ContainerOfCustomWidgets container;
	container.items.push_back(S_CustomWidgetTestType{});

	rttr::instance containerInst(container);
	const auto	   type = containerInst.get_type().get_raw_type();
	const auto	   prop = type.get_property("Items");
	ASSERT_TRUE(prop.is_valid());

	auto var  = prop.get_value(container);
	auto view = var.create_sequential_view();
	ASSERT_EQ(view.get_size(), 1u);

	rttr::variant  elementVar = view.get_value(0);
	rttr::instance elementInst(elementVar);
	rttr::instance unwrapped = elementInst.get_type().get_raw_type().is_wrapper() ? elementInst.get_wrapped_instance() : elementInst;

	EXPECT_TRUE(UI::IsTypeUIMetaClass<MetaGUI::CustomGUIWidget>(unwrapped));
}

} // namespace GLEngine::Renderer
