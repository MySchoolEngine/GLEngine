#include <RendererStdafx.h>

#include <Renderer/RayCasting/Frame.h>
#include <Renderer/RayCasting/ReflectionModels/OrenNayar.h>
#include <Renderer/RayCasting/Sampling.h>

namespace GLEngine::Renderer {

//=================================================================================
C_OrenNayarModel::C_OrenNayarModel(const Colours::T_Colour& colour, float sigmaDeg)
	: m_Colour(colour)
{
	const float sigmaRad = glm::radians(sigmaDeg);
	const float sigma2	 = sigmaRad * sigmaRad;
	A					 = 1.f - (sigma2 / (2.f * (sigma2 + 0.33f)));
	B					 = 0.45f * sigma2 / (sigma2 + 0.09f);
}

//=================================================================================
C_OrenNayarModel::~C_OrenNayarModel() = default;

//=================================================================================
Colours::T_Colour C_OrenNayarModel::f(const glm::vec3& wi, const glm::vec3& wo) const
{
	S_Frame		defaultFrame;
	const float sinThetaI = defaultFrame.SinTheta(wi);
	const float sinThetaO = defaultFrame.SinTheta(wo);

	float maxCos = 0.f;
	if (sinThetaI > 1e-4 && sinThetaO > 1e-4)
	{
		const float sinPhiI = defaultFrame.SinPhi(wi);
		const float cosPhiI = defaultFrame.CosPhi(wi);
		const float sinPhiO = defaultFrame.SinPhi(wo);
		const float cosPhiO = defaultFrame.CosPhi(wo);
		const float dCos	= cosPhiI * cosPhiO + sinPhiI * sinPhiO;
		maxCos				= std::max(0.0f, dCos);
	}

	float sinAlpha = 0.f;
	float tanBeta  = 0.f;
	const float absCosThetaI = defaultFrame.AbsCosTheta(wi);
	const float absCosThetaO = defaultFrame.AbsCosTheta(wo);

	if (absCosThetaI > absCosThetaO)
	{
		sinAlpha = sinThetaO;
		tanBeta	 = sinThetaI / absCosThetaI;
	}
	else
	{
		sinAlpha = sinThetaI;
		tanBeta	 = sinThetaO / absCosThetaO;
	}

	return m_Colour * glm::one_over_pi<float>() * (A + B * maxCos * sinAlpha * tanBeta);
}

//=================================================================================
Colours::T_Colour C_OrenNayarModel::SampleF(const glm::vec3& wi, glm::vec3& wo, const S_Frame& frame, const glm::vec2& rng, float* pdf) const
{
	wo = CosineSampleHemisphere(rng);
	*pdf = Pdf(wi, wo);
	return f(wi, wo);
}

//=================================================================================
float C_OrenNayarModel::Pdf(const glm::vec3& wi, const glm::vec3& wo) const
{
	return CosineSampleHemispherePDF(wo.y);
}

} // namespace GLEngine::Renderer