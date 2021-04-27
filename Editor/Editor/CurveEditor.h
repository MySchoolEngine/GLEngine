#pragma once

#include <Editor/EditorApi.h>

#include <Core/EventSystem/EventReciever.h>

namespace GLEngine::Renderer {
class C_Curve;
class I_DebugDraw;
} // namespace GLEngine::Renderer

namespace GLEngine::Editor {
class EDITOR_API_EXPORT C_CurveEditor : public Core::I_EventReciever {
public:
	C_CurveEditor(Renderer::C_Curve& curve);

	virtual void OnEvent(Core::I_Event& event) override;
	void		 OnUpdate(const Core::I_Input& input);

	void Draw(Renderer::I_DebugDraw& dd) const;

private:
	Renderer::C_Curve& m_Curve;
};
} // namespace GLEngine::Editor