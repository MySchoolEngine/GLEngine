#include <RendererStdafx.h>

#include <Renderer/RayCasting/Frame.h>
#include <Renderer/RayCasting/ReflectionModels/SpecularReflection.h>

namespace GLEngine::Renderer {

//=================================================================================
C_SpecularReflection::C_SpecularReflection(float etaI, float etaO)
	: m_etaI(etaI)
	, m_etaO(etaO)
{
}

//=================================================================================
glm::vec3 C_SpecularReflection::f(const glm::vec3& wi, const glm::vec3& wo) const
{
	// as far as I understated this, in reflection model we have exactly one direction in which
	// F() != 0 and probability of hitting this exact direction is equal to 0
	return glm::vec3(0.0f);
}

//=================================================================================
glm::vec3 C_SpecularReflection::SampleF(const glm::vec3& wi, glm::vec3& wo, const S_Frame& frame, const glm::vec2& rng, float* pdf) const
{
	wo	 = frame.Reflect(wi);
	*pdf = 1.0f;
	return glm::vec3(FresnelDieletrics(frame.CosTheta(wi), m_etaI, m_etaO) / frame.AbsCosTheta(wo));
}

//=================================================================================
float C_SpecularReflection::Pdf(const glm::vec3& wi, const glm::vec3& wo) const
{
	return 0.0f;
}

} // namespace GLEngine::Renderer