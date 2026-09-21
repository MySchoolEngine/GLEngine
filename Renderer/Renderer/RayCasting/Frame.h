#pragma once

#include <glm/glm.hpp>

#include <algorithm>
#include <cmath>

namespace GLEngine::Renderer {

// Structure to simplify surface interaction code
// Creates reference frame using orthonormal basis with (0,1,0) local coordinate
// as its normal
// | y / z
// |  /
// | /
// |/     x
// +-------
struct S_Frame {
public:
	// default constructed frame has normal facing along Y
	constexpr S_Frame();
	constexpr S_Frame(const glm::vec3& x, const glm::vec3& y, const glm::vec3& z);
	S_Frame(const glm::vec3& normal);

	void SetFromNormal(const glm::vec3& normal);

	[[nodiscard]] constexpr glm::vec3 ToWorld(const glm::vec3& a) const;
	[[nodiscard]] glm::vec3			  ToLocal(const glm::vec3& a) const;

	[[nodiscard]] constexpr glm::vec3 Normal() const;
	[[nodiscard]] constexpr glm::vec3 Tangnt() const;
	[[nodiscard]] constexpr glm::vec3 Bitangent() const;

	// w is in local coords
	[[nodiscard]] static constexpr float CosTheta(const glm::vec3& w) { return w.y; }
	[[nodiscard]] static constexpr float Cos2Theta(const glm::vec3& w) { return w.y * w.y; }
	[[nodiscard]] static float			 AbsCosTheta(const glm::vec3& w) { return std::abs(w.y); }

	[[nodiscard]] static float			 SinTheta(const glm::vec3& w) { return std::sqrt(Sin2Theta(w)); }
	[[nodiscard]] static constexpr float Sin2Theta(const glm::vec3& w) { return std::max(0.f, 1.f - Cos2Theta(w)); }

	[[nodiscard]] static float			 TanTheta(const glm::vec3& w) { return SinTheta(w) / CosTheta(w); }
	[[nodiscard]] static constexpr float Tan2Theta(const glm::vec3& w) { return Sin2Theta(w) / Cos2Theta(w); }

	[[nodiscard]] static float SinPhi(const glm::vec3& w)
	{
		const float sinTheta = SinTheta(w);
		return sinTheta == 0.f ? 0.0f : glm::clamp(w.y / sinTheta, -1.f, 1.f);
	}
	[[nodiscard]] static float CosPhi(const glm::vec3& w)
	{
		const float sinTheta = SinTheta(w);
		return sinTheta == 0.f ? 0.0f : glm::clamp(w.x / sinTheta, -1.f, 1.f);
	}

	/**
	 * @brief Works for vectors in local space and with respect to frames normal. Tests if both are on the same side
	 * of the geometry.
	 */
	[[nodiscard]] static constexpr bool IsSameHemisphere(const glm::vec3& x, const glm::vec3& y) { return x.y * y.y > 0.f; }

	[[nodiscard]] static glm::vec3 Reflect(const glm::vec3& wi) { return glm::vec3(-wi.x, wi.y, -wi.z); }

	void Transform(const glm::mat4& mat)
	{
		const auto transposed = glm::transpose(mat);
		X					  = transposed * glm::vec4(X, 0.f);
		Y					  = transposed * glm::vec4(Y, 0.f);
		Z					  = transposed * glm::vec4(Z, 0.f);
	}

private:
	glm::vec3 X;
	glm::vec3 Y;
	glm::vec3 Z;
};
} // namespace GLEngine::Renderer

#include <Renderer/RayCasting/Frame.inl>
