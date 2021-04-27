#pragma once

#include <Editor/EditorApi.h>

#include <Core/EventSystem/EventReciever.h>

namespace GLEngine::Core {
class I_Input;
}

namespace GLEngine::Renderer {
class C_Curve;
class I_DebugDraw;
class I_CameraComponent;
} // namespace GLEngine::Renderer

namespace GLEngine::Editor {
class EDITOR_API_EXPORT C_CurveEditor : public Core::I_EventReciever {
public:
	C_CurveEditor(Renderer::C_Curve& curve);

	virtual void OnEvent(Core::I_Event& event) override;
	void		 OnUpdate(const Core::I_Input& input, std::shared_ptr<Renderer::I_CameraComponent> camera);

	void Draw(Renderer::I_DebugDraw& dd) const;

private:
	Renderer::C_Curve& m_Curve;

	int m_SelectedPoint;
};
} // namespace GLEngine::Editor