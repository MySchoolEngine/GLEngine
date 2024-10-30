#pragma once

#include <Editor/Editors/ImageEditorTool.h>

#include <Renderer/Colours.h>
#include <Renderer/Textures/TextureView.h>

#include <rttr/registration.h>
#include <rttr/registration_friend.h>

namespace GLEngine::Editor {
class C_BrickGenerator : public C_ImageEditorTool {
public:
	C_BrickGenerator(Renderer::C_TextureView view);
	RTTR_ENABLE(C_ImageEditorTool);
	RTTR_REGISTRATION_FRIEND;

	void Apply() override;

private:
	int m_RowHeight;
	int m_BrickWidth;
	int m_MortarThickness;
	int m_BrickOffset;

	// randomness
	int m_RowHeightDeviation;
	int m_BrickWidthDeviation;
	int m_MortarWidthDeviation;
	int m_MortarHeightDeviation;

	Colours::T_Colour m_BrickColour;
	Colours::T_Colour m_MortarColour;
};
} // namespace GLEngine::Editor