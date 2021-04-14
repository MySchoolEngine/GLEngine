#pragma once

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
	[[nodiscard]] constexpr float CosTheta(const glm::vec3& w) const { return w.y; }
	[[nodiscard]] constexpr float Cos2Theta(const glm::vec3& w) const { return w.y * w.y; }
	[[nodiscard]] float			  AbsCosTheta(const glm::vec3& w) const { return std::abs(w.y); }

	[[nodiscard]] float			  SinTheta(const glm::vec3& w) const { return std::sqrt(Sin2Theta(w)); }
	[[nodiscard]] constexpr float Sin2Theta(const glm::vec3& w) const { return std::max(0.f, 1.f - Cos2Theta(w)); }

	[[nodiscard]] float TanTheta(const glm::vec3& w) const { return SinTheta(w) / CosTheta(w); }
	[[nodiscard]] float Tan2Theta(const glm::vec3& w) const { return Sin2Theta(w) / Cos2Theta(w); }

	[[nodiscard]] glm::vec3 Reflect(const glm::vec3& wi) const { return glm::vec3(-wi.x, wi.y, -wi.z); }

private:
	glm::vec3 X;
	glm::vec3 Y;
	glm::vec3 Z;
};
} // namespace GLEngine::Renderer

#include <Renderer/RayCasting/Frame.inl>
