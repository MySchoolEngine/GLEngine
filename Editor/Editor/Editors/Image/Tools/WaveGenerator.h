#pragma once

#include <Editor/Editors/ImageEditorTool.h>

#include <Renderer/Colours.h>
#include <Renderer/Textures/TextureView.h>

#include <rttr/registration.h>
#include <rttr/registration_friend.h>

namespace GLEngine::Editor {
class C_WaveGenerator : public C_ImageEditorTool {
public:
	C_WaveGenerator(Renderer::C_TextureView view);
	void Apply() override;

	RTTR_ENABLE(C_ImageEditorTool)
	RTTR_REGISTRATION_FRIEND
private:
	glm::ivec2 m_Center;
	float	   m_WaveSize;
};
} // namespace GLEngine::Editor