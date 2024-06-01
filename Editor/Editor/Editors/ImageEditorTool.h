#pragma once

#include <Renderer/Colours.h>
#include <Renderer/Textures/TextureView.h>

#include <rttr/registration.h>
#include <rttr/registration_friend.h>

namespace GLEngine::Editor {
// event system should handle resource update? Or should I propagate it otherwise?
// There is no resource so just propagate to the editor and editor should handle
// this.
class C_ImageEditorTool {
public:
	// reflection GUI create the view
	// needs access to the storage
	C_ImageEditorTool(Renderer::C_TextureView view); // can be performed on whole image or just part of it

	virtual void Apply() {}
	// for reflection GUI
	RTTR_ENABLE();

protected:
	Renderer::C_TextureView m_View;
};

class C_GaussianBlur : public C_ImageEditorTool {
public:
	C_GaussianBlur(Renderer::C_TextureView view);
	RTTR_ENABLE(C_ImageEditorTool);
	RTTR_REGISTRATION_FRIEND;

	void Apply() override;

private:
	/*std::uint8_t*/ int m_KernelSize;
	float				 m_Sigma;
};
} // namespace GLEngine::Editor