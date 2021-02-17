#pragma once

#include <glm/glm.hpp>

namespace GLEngine::Renderer::RayTracing {
class I_RayLight {
public:
	virtual ~I_RayLight()							 = default;
	[[nodiscard]] virtual glm::vec3 SampleLi() const = 0;
	[[nodiscard]] virtual glm::vec3 Le() const		 = 0;
	[[nodiscard]] virtual float		PdfLe() const	 = 0;
};
} // namespace GLEngine::Renderer::RayTracing