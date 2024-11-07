#pragma once

#include <Renderer/Colours.h>
#include <Renderer/RayCasting/ReflectionModels/IReflectionModel.h>

namespace GLEngine::Renderer {
struct S_Frame;

class C_OrenNayarModel : public I_ReflectionModel {
public:
	C_OrenNayarModel(const Colours::T_Colour& colour, float sigmaDeg);
	~C_OrenNayarModel();

	[[nodiscard]] virtual Colours::T_Colour f(const glm::vec3& wi, const glm::vec3& wo) const override;
	[[nodiscard]] virtual Colours::T_Colour SampleF(const glm::vec3& wi, glm::vec3& wo, const S_Frame& frame, const glm::vec2& rng, float* pdf) const override;
	[[nodiscard]] virtual float				Pdf(const glm::vec3& wi, const glm::vec3& wo) const override;

private:
	Colours::T_Colour m_Colour;
	float			  A, B;
};
} // namespace GLEngine::Renderer