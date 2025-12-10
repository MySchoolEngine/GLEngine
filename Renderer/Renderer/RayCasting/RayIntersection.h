#pragma once

#include <Renderer/RayCasting/Frame.h>
#include <Renderer/RayCasting/Material/MaterialInterface.h>
#include <Renderer/RendererApi.h>

#include <Physics/Primitives/Ray.h>

namespace GLEngine::Renderer {
namespace RayTracing {
class I_RayLight;
}
class RENDERER_API_EXPORT C_RayIntersection {
public:
	C_RayIntersection(); //= default;
	C_RayIntersection(const S_Frame& frame, const glm::vec3& point, const Physics::Primitives::S_Ray& ray);
	~C_RayIntersection(); //= default;

	// Explicitly default copy/move operations (or delete if not needed)
	C_RayIntersection(const C_RayIntersection&)		= default;
	C_RayIntersection(C_RayIntersection&&) noexcept = default;
	C_RayIntersection& operator=(const C_RayIntersection&) = default;
	C_RayIntersection& operator=(C_RayIntersection&&) noexcept = default;

	[[nodiscard]] const S_Frame&					GetFrame() const;
	[[nodiscard]] const Physics::Primitives::S_Ray& GetRay() const;
	[[nodiscard]] const glm::vec3&					GetIntersectionPoint() const;
	/**
	 * @brief
	 * @return Length from the \var m_Ray.origin to intersection, infinity if ray missed
	 */
	[[nodiscard]] float GetRayLength() const;
	void				SetRayLength(float t) { m_RayLength = t; }

	/**
	 * @brief Do not transfer the ownership
	 * @param material 
	 */
	void									 SetMaterial(const I_MaterialInterface* material);
	[[nodiscard]] const I_MaterialInterface* GetMaterial() const;

	[[nodiscard]] bool											 IsLight() const;
	void														 SetLight(const std::shared_ptr<RayTracing::I_RayLight>& light);
	void														 SetLight(std::shared_ptr<RayTracing::I_RayLight>&& light);
	[[nodiscard]] const std::shared_ptr<RayTracing::I_RayLight>& GetLight() const;

	[[nodiscard]] glm::vec2 GetUV() const { return m_UV; }
	void					SetUV(const glm::vec2& UV) { m_UV = UV; }

	void TransformRayAndPoint(const glm::mat4& mat);

private:
	S_Frame									m_Frame;
	glm::vec3								m_Point;
	Physics::Primitives::S_Ray				m_Ray;
	glm::vec2								m_UV		= {0.f, 0.f};
	float									m_RayLength = std::numeric_limits<float>::infinity();
	const I_MaterialInterface*				m_Material	= nullptr; // not owning
	std::shared_ptr<RayTracing::I_RayLight> m_Light		= nullptr;
};
} // namespace GLEngine::Renderer
