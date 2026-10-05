#pragma once

#include <Renderer/Colours.h>
#include <DULib/BitField.h>

namespace GLEngine::Renderer {

struct S_Frame;

class I_ReflectionModel {
public:
	enum class Type : std::uint8_t {
		Reflection = BIT(0),
		Transmission = BIT(1)
	};
	virtual ~I_ReflectionModel() = default;

	// Parameter: const glm::vec3 & wi - In local frame space
	// Parameter: const glm::vec3 & wo - In local frame space
	// Reflection model itself is responsible for discarding of samples that are not physically plausible
	// for given model. e.g. transmissive samples for opaque models
	[[nodiscard]] virtual Colours::T_Colour f(const glm::vec3& wi, const glm::vec3& wo) const														  = 0;
	[[nodiscard]] virtual Colours::T_Colour SampleF(const glm::vec3& wi, glm::vec3& wo, const S_Frame& frame, const glm::vec2& rng, float* pdf) const = 0;
	[[nodiscard]] virtual float				Pdf(const glm::vec3& wi, const glm::vec3& wo) const														  = 0;

	[[nodiscard]] constexpr virtual DULib::BitField<Type> GetType() = 0;

protected:
	static float FresnelDieletrics(float cosThetaI, float etaI, float etaO);
};
} // namespace GLEngine::Renderer

template <> struct DULib::enable_BitField_operators<GLEngine::Renderer::I_ReflectionModel::Type> {
	static constexpr bool enable = true;
};

template <> struct DULib::BitField_UsedBitsCounter<GLEngine::Renderer::I_ReflectionModel::Type> {
	static constexpr std::size_t usedBits = 2;
};