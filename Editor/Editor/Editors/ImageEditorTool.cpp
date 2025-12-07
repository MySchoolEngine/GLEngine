#include <EditorStdafx.h>

#include <Editor/Editors/ImageEditorTool.h>

#include <GUI/ReflectionGUI.h>

#include <rttr/registration>

RTTR_REGISTRATION
{
	using namespace GLEngine::Editor;
	using namespace Utils::Reflection;

	rttr::registration::class_<C_ImageEditorTool>("C_ImageEditorTool");
	rttr::registration::class_<C_GaussianBlur>("C_GaussianBlur")
		.property("Sigma", &C_GaussianBlur::m_Sigma)(
			rttr::policy::prop::as_reference_wrapper,
			RegisterMetaclass<MetaGUI::Slider>(),
			RegisterMetamember<UI::Slider::Name>("Sigma:"),
			RegisterMetamember<UI::Slider::Min>(0.001f),
			RegisterMetamember<UI::Slider::Max>(1.0f));
}

namespace GLEngine::Editor {


//=================================================================================
C_ImageEditorTool::C_ImageEditorTool(Renderer::C_TextureView view)
	: m_View(view)
{
}

//=================================================================================
C_GaussianBlur::C_GaussianBlur(Renderer::C_TextureView view)
	: C_ImageEditorTool(view)
	, m_KernelSize(5)
	, m_Sigma(0.1f)
{
}

//=================================================================================
void C_GaussianBlur::Apply()
{
}

} // namespace GLEngine::Editor