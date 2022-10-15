#include <RendererStdafx.h>

#include <Renderer/RayCasting/ReflectionModels/LambertianModel.h>
#include <Renderer/RayCasting/Sampling.h>

namespace GLEngine::Renderer {

//=================================================================================
C_LambertianModel::C_LambertianModel(glm::vec3& color)
	: m_Colour(color)
{
}

//=================================================================================
C_LambertianModel::~C_LambertianModel() = default;

//=================================================================================
glm::vec3 C_LambertianModel::f(const glm::vec3& wi, const glm::vec3& wo) const
{
	return m_Colour * glm::one_over_pi<float>();
}

//=================================================================================
glm::vec3 C_LambertianModel::SampleF(const glm::vec3& wi, glm::vec3& wo, const S_Frame& frame, const glm::vec2& rng, float* pdf) const
{
	wo = CosineSampleHemisphere(rng);

	*pdf = Pdf(wi, wo);

	return f(wi, wo);
}

//=================================================================================
float C_LambertianModel::Pdf(const glm::vec3& wi, const glm::vec3& wo) const
{
	return CosineSampleHemispherePDF(wo.y);
}

} // namespace GLEngine::Renderer