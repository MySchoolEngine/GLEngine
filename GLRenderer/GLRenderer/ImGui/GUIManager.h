#pragma once

namespace GLEngine::GLRenderer::GUI {
class C_Window;
}

namespace GLEngine::GLRenderer::ImGui {
class C_GUIManager
{
public:
	C_GUIManager();
	~C_GUIManager();

	GUID CreateGUIWindow(const std::string& name);
	void AddCustomWindow(GUI::C_Window* window);

	// modal windows coming

	GUI::C_Window* GetWindow(GUID guid) const;

	void DestroyWindow(GUID guid);
	void OnUpdate();

private:
	std::unordered_map<GUID, GUI::C_Window*> m_Windwos;
};
}

