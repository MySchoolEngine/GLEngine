#include <RendererStdAfx.h>

#include <Renderer/RayCasting/Generator/Sampler.h>

namespace GLEngine::Renderer {

//=================================================================================
C_STDSampler::C_STDSampler(float min, float max)
	: m_Distrib(min, max)
{
	std::random_device		  rd;
	std::mt19937::result_type seed
		= rd()
		  ^ ((std::mt19937::result_type)std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count()
			 + (std::mt19937::result_type)std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count());

	m_Gen = std::mt19937(seed);
}

//=================================================================================
float C_STDSampler::GetD()
{
	return m_Distrib(m_Gen);
}

//=================================================================================
glm::vec2 C_STDSampler::GetV2()
{
	return {m_Distrib(m_Gen), m_Distrib(m_Gen)};
}

//=================================================================================
glm::vec3 C_STDSampler::GetV3()
{
	return {m_Distrib(m_Gen), m_Distrib(m_Gen), m_Distrib(m_Gen)};
}

} // namespace GLEngine::Renderer
