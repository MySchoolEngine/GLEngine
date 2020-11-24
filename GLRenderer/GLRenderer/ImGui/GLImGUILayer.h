#pragma once

#include <GUI/ImGuiLayer.h>

namespace GLEngine::GLRenderer {
class C_GLImGUILayer : public GUI::C_ImGuiLayer
{
public:
	C_GLImGUILayer(GUID window);
	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void FrameBegin() override;
	virtual void FrameEnd() override;

};
}

