#pragma once

#include <Renderer/RayCasting/ReflectionModels/IReflectionModel.h>

namespace GLEngine::Renderer {

class C_ScaledReflectanceModel : public I_ReflectionModel {
public:
	C_ScaledReflectanceModel(I_ReflectionModel* model, float scale);


	[[nodiscard]] Colours::T_Colour f(const glm::vec3& wi, const glm::vec3& wo) const override;
	[[nodiscard]] Colours::T_Colour SampleF(const glm::vec3& wi, glm::vec3& wo, const S_Frame& frame, const glm::vec2& rng, float* pdf) const override;
	[[nodiscard]] float				Pdf(const glm::vec3& wi, const glm::vec3& wo) const override;

private:
	I_ReflectionModel* m_Model; //< Not owning, allocated by arena allocator
	float			   m_Scale;
};
} // namespace GLEngine::Renderer
