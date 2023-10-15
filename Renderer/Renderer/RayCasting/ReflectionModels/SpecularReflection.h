#pragma once

#include <Renderer/RayCasting/ReflectionModels/IReflectionModel.h>


// so far I implemented dielectric reflection, so all I need is to know the eta of
// current media and the media around the border of medias
namespace GLEngine::Renderer {

struct S_Frame;

class C_SpecularReflection : public I_ReflectionModel {
public:
	C_SpecularReflection(float etaI, float etaO);

	[[nodiscard]] virtual Colours::T_Colour f(const glm::vec3& wi, const glm::vec3& wo) const override;
	[[nodiscard]] virtual Colours::T_Colour SampleF(const glm::vec3& wi, glm::vec3& wo, const S_Frame& frame, const glm::vec2& rng, float* pdf) const override;
	[[nodiscard]] virtual float				Pdf(const glm::vec3& wi, const glm::vec3& wo) const override;

private:
	float m_etaI;
	float m_etaO;
};
} // namespace GLEngine::Renderer
