#pragma once


namespace GLEngine::GLRenderer::GUI {
class I_GUIPart;


class C_Window {
public:
	C_Window(std::string&& name);

	void Activate(bool activate = true);
	void Deactivate();
	
	bool IsActive() const;

	void Draw() const;

	void DrawElements() const;

	void Begin() const;
	void End() const;

	void AddGUIPart(I_GUIPart* part);
protected:
	std::string m_Name;
	std::vector<I_GUIPart*> m_Elements;
	mutable bool	m_Active;
};

}