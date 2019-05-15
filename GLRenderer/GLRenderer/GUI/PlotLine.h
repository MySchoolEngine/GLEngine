#pragma once

#include <GLRendererStdafx.h>
#include <GLRenderer/GUI/GUIComponent.h>

namespace GLEngine {
namespace GLRenderer {
namespace GUI {

template<int samples>
class C_PlotLine : public I_GUIComponent{
	using T_SamplesArray = std::array<float, samples>;
	using T_SamplesCIter = typename T_SamplesArray::const_iterator;
public:
	C_PlotLine(std::string&& name);
	virtual void Draw() const override;
	void Sample(float sample);
	float Avg() const;
	float LastSample() const;
	
	// to allow std algorithms work on this
	T_SamplesCIter cbegin() const;
	T_SamplesCIter cend() const;
protected:
	T_SamplesArray				m_Samples;
	std::string					m_Name;
	int							m_SampleID;
};

}}}

#include <GLRenderer/GUI/PlotLine.inl>