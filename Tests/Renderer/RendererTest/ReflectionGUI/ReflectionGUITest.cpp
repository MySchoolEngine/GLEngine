#include <RendererTestStdafx.h>

#include <GUI/ReflectionGUI.h>

#include <Utils/Reflection/Metadata.h>

#include <rttr/registration>

namespace GLEngine::Renderer {

namespace {

struct S_CustomWidgetTestType {
	int value = 0;
};

bool g_DrawFunctionCalled		= false;
bool g_DrawFunctionReturnValue = false;

bool TestDrawFunction(rttr::instance&)
{
	g_DrawFunctionCalled = true;
	return g_DrawFunctionReturnValue;
}

} // namespace

} // namespace GLEngine::Renderer

RTTR_REGISTRATION
{
	using namespace ::Utils::Reflection;
	rttr::registration::class_<GLEngine::Renderer::S_CustomWidgetTestType>("S_CustomWidgetTestType")
	(
		RegisterMetaclass<MetaGUI::CustomGUIWidget>(),
		RegisterMetamember<UI::CustomGUIWidget::DrawFunction>(std::function<bool(rttr::instance&)>(&GLEngine::Renderer::TestDrawFunction))
	)
	.constructor<>()(rttr::policy::ctor::as_object)
	.property("Value", &GLEngine::Renderer::S_CustomWidgetTestType::value);
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

} // namespace GLEngine::Renderer
