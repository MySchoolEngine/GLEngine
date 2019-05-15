#pragma once

#include <GLRendererStdafx.h>
#include <GLRenderer/GUI/GUIComponent.h>

namespace GLEngine {
namespace GLRenderer {
namespace GUI {

template<int samples>
class C_PlotLine : public I_GUIComponent{
public:
	C_PlotLine(std::string&& name);
	virtual void Draw() const override;
	void Sample(float sample);
	float Avg() const;
	float LastSample() const;
protected:
	std::array<float, samples>	m_Samples;
	std::string					m_Name;
	int							m_SampleID;
};

}}}

#include <GLRenderer/GUI/PlotLine.inl>