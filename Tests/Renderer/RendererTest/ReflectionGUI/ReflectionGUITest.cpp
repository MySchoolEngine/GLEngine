#include <RendererTestStdafx.h>

#include <GUI/ReflectionGUI.h>

#include <Utils/Reflection/Metadata.h>

#include <rttr/registration>

namespace GLEngine::Renderer {

namespace {

struct S_CustomWidgetTestType {
	int value = 0;
};

bool g_DrawFunctionCalled = false;

void TestDrawFunction(rttr::instance&)
{
	g_DrawFunctionCalled = true;
}

} // namespace

} // namespace GLEngine::Renderer

RTTR_REGISTRATION
{
	using namespace ::Utils::Reflection;
	rttr::registration::class_<GLEngine::Renderer::S_CustomWidgetTestType>("S_CustomWidgetTestType")
	(
		RegisterMetaclass<MetaGUI::CustomGUIWidget>(),
		RegisterMetamember<UI::CustomGUIWidget::DrawFunction>(std::function<void(rttr::instance&)>(&GLEngine::Renderer::TestDrawFunction))
	)
	.constructor<>()(rttr::policy::ctor::as_object)
	.property("Value", &GLEngine::Renderer::S_CustomWidgetTestType::value);
}

namespace GLEngine::Renderer {

TEST(ReflectionGUI, DrawAllPropertyGUI_CustomWidgetRegisteredOnWrappedInstance_IsFound)
{
	g_DrawFunctionCalled = false;

	S_CustomWidgetTestType value;
	// std::reference_wrapper is the exact wrapper scenario the bug was about: get_type() on it
	// returns the wrapper's own type, not S_CustomWidgetTestType's, so the custom-widget metadata
	// lookup must unwrap first or it silently falls through to the generic property-drawing path.
	std::reference_wrapper<S_CustomWidgetTestType> wrapped(value);
	rttr::instance									 obj(wrapped);

	const auto changedProps = GUI::DrawAllPropertyGUI(obj);

	EXPECT_TRUE(g_DrawFunctionCalled);
	EXPECT_TRUE(changedProps.empty()); // the custom-widget path returns early, never walking properties
}

} // namespace GLEngine::Renderer
