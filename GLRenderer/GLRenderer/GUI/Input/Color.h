#pragma once

#include <GLRenderer/GUI/GUIPart.h>

namespace GLEngine::GLRenderer::GUI::Input {
class C_ColorRBG : public I_GUIPart {
public:
	C_ColorRBG(std::string&& name, glm::vec3 default);

	virtual void Draw() const override;
	const glm::vec3& GetValue() const { return m_Color; }
private:
	std::string				m_name;
	mutable glm::vec3	m_Color;
};


class C_ColorRBGA : public I_GUIPart {
public:
	C_ColorRBGA(std::string&& name, glm::vec4 default);

	virtual void Draw() const override;
	const glm::vec4& GetValue() const { return m_Color; }
private:
	std::string				 m_name;
	mutable  glm::vec4 m_Color;
};
}