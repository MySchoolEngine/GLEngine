#pragma once

#include <GUI/ImGuiLayer.h>

namespace GLEngine::GLRenderer {
class C_GLImGUILayer : public GUI::C_ImGuiLayer {
public:
	explicit C_GLImGUILayer(GUID window);
	void OnAttach() override;
	void OnDetach() override;

	void FrameBegin() override;
	void FrameEnd(Core::I_Input& input) override;
};
} // namespace GLEngine::GLRenderer
