#pragma once

#include <Editor/Editors/ImageEditorTool.h>

#include <Renderer/Textures/TextureView.h>

#include <rttr/registration.h>
#include <rttr/registration_friend.h>

namespace GLEngine::Editor {
class C_PerlinNoise : public C_ImageEditorTool {
public:
	C_PerlinNoise(Renderer::C_TextureView view);
	RTTR_ENABLE(C_ImageEditorTool);
	RTTR_REGISTRATION_FRIEND;

	void Apply() override;

private:
	float PNoise(const glm::uvec2& coord, int res);
	float Noise(const glm::uvec2& coord, float freq);

	int		m_Frequency = 1;
	float	m_Persistance = 0.5;
};

} // namespace GLEngine::Editor