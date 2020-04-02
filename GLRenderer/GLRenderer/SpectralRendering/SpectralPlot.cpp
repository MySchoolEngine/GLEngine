#include <GLRendererStdafx.h>

#include <GLRenderer/SpectralRendering/SpectralPlot.h>


namespace GLEngine::GLRenderer::SpectralRendering {

//=================================================================================
C_SpectralPlot::C_SpectralPlot(std::string name)
	: m_Name(name)
	, m_Tooltip("wavelength: %g\nintensity: %8.4g")
{}

//=================================================================================
C_SpectralPlot::~C_SpectralPlot() = default;

//=================================================================================
void C_SpectralPlot::Draw() const
{
	std::vector<const float*> yData;
	for (const auto& it : m_yData)
	{
		yData.emplace_back(it.data());
	}

	std::vector< ImU32> colors;
	for (const auto& it : m_Color)
	{
		ImColor col(it.x * 255, it.y * 255, it.z * 255);
		colors.emplace_back(col);
	}

	// Draw first plot with multiple sources
	::ImGui::PlotConfig conf;
	conf.values.xs = m_xData.data();
	conf.values.count = static_cast<int>(m_xData.size());

	if (m_yData.empty())
	{
		return;
	}
	if (m_yData.size() == 1) {
		conf.values.ys = m_yData[0].data();
		conf.values.color = ImColor(m_Color[0].x * 255, m_Color[0].y * 255, m_Color[0].z * 255);


		const auto min = std::min_element(m_yData[0].begin(), m_yData[0].end());
		const auto max = std::max_element(m_yData[0].begin(), m_yData[0].end());
		conf.scale.min = *min;
		conf.scale.max = *max;
	}
	else
	{
		conf.values.ys_list = yData.data();
		conf.values.colors = colors.data();

		std::vector<float> mins;
		std::vector<float> maxs;
		for (const auto& it : m_yData)
		{
			const auto min = std::min_element(it.begin(), it.end());
			const auto max = std::max_element(it.begin(), it.end());
			mins.push_back(*min);
			maxs.push_back(*max);
		}
		const auto min = std::min_element(mins.begin(), mins.end());
		const auto max = std::max_element(maxs.begin(), maxs.end());

		conf.scale.min = *min;
		conf.scale.max = *max;
	}


	conf.values.ys_count = static_cast<int>(m_yData.size());
	conf.tooltip.show = true;
	conf.tooltip.format = m_Tooltip.c_str();
	conf.grid_x.show = true;
	conf.grid_x.size = 128;
	conf.grid_x.subticks = 4;
	conf.grid_y.show = true;
	conf.grid_y.size = 0.5f;
	conf.grid_y.subticks = 5;
	conf.frame_size = ImVec2(200, 200);
	::ImGui::Plot(m_Name.c_str(), conf);
}

}