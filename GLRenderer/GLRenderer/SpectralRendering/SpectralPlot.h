#pragma once

#include <GLRenderer/GUI/GUIPart.h>

namespace GLEngine::GLRenderer::SpectralRendering {
class C_SpectralPlot : public GUI::I_GUIPart
{
public:
	C_SpectralPlot(std::string name);
	~C_SpectralPlot();

	//=================================================================================
	void Draw() const override;

	std::string											m_Name;
	std::string											m_Tooltip;
	std::vector<glm::vec3>					m_Color;
	std::vector<float>							m_xData;
	std::vector<std::vector<float>>	m_yData;
};
}