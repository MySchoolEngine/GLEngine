#pragma once

#include <Renderer/RayCasting/ReflectionModels/IReflectionModel.h>

namespace GLEngine::Renderer {
struct S_Frame;

class C_LambertianModel : public I_ReflectionModel {
public:
	C_LambertianModel(glm::vec3& color);
	~C_LambertianModel() override;

	[[nodiscard]] Colours::T_Colour f(const glm::vec3& wi, const glm::vec3& wo) const override;
	[[nodiscard]] Colours::T_Colour SampleF(const glm::vec3& wi, glm::vec3& wo, const S_Frame& frame, const glm::vec2& rng, float* pdf) const override;
	[[nodiscard]] float				Pdf(const glm::vec3& wi, const glm::vec3& wo) const override;

	[[nodiscard]] constexpr DULib::BitField<Type> GetType() override { return Type::Reflection; }

private:
	glm::vec3 m_Colour;
};
} // namespace GLEngine::Renderer