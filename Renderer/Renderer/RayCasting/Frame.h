#pragma once


namespace GLEngine::Renderer {

struct S_Frame
{
public:
	constexpr S_Frame();
	constexpr S_Frame(
		const glm::vec3& x,
		const glm::vec3& y,
		const glm::vec3& z);
	S_Frame(const glm::vec3& normal);

	void SetFromNormal(const glm::vec3& normal);

	[[nodiscard]] constexpr glm::vec3 ToWorld(const glm::vec3& a) const;
	[[nodiscard]] constexpr glm::vec3 ToLocal(const glm::vec3& a) const;

	[[nodiscard]] constexpr glm::vec3 Normal() const;
	[[nodiscard]] constexpr glm::vec3 Tangnt() const;
	[[nodiscard]] constexpr glm::vec3 Bitangent() const;
private:
	glm::vec3 X;
	glm::vec3 Y;
	glm::vec3 Z;
};
}

#include <Renderer/RayCasting/Frame.inl>
