#include <RendererStdafx.h>

#include <Renderer/RayCasting/ReflectionModels/IReflectionModel.h>


namespace GLEngine::Renderer {

//=================================================================================
float I_ReflectionModel::FresnelDieletrics(float cosThetaI, float etaI, float etaO)
{
	const auto entering = cosThetaI > 0.f;
	if (entering)
	{
		std::swap(etaI, etaO);
		cosThetaI = std::abs(cosThetaI);
	}

	const auto sinThetaI = std::sqrt(std::max(0.f, 1 - cosThetaI * cosThetaI));
	// literally Snell's law
	const auto sinThetaO = etaI / etaO * sinThetaI;

	// total internal reflection
	if (sinThetaO >= 1.f)
	{
		return 1.f;
	}

	const auto cosThetaO = std::sqrt(std::max(0.f, 1 - cosThetaI * cosThetaI));

	const auto Rparallel	  = ((etaO * cosThetaI) - (etaI * cosThetaO)) / ((etaO * cosThetaI) + (etaI * cosThetaO));
	const auto Rperpendicular = ((etaI * cosThetaI) - (etaO * cosThetaO)) / ((etaI * cosThetaI) + (etaO * cosThetaO));
	return (Rparallel * Rparallel + Rperpendicular * Rperpendicular) / 2;
}

} // namespace GLEngine::Renderer
