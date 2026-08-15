#pragma once

#include <Renderer/Colours.h>
#include <Renderer/RendererApi.h>

namespace GLEngine::Renderer {
class I_Sampler;
class C_RayIntersection;
} // namespace GLEngine::Renderer

namespace GLEngine::Renderer::RayTracing {

enum class LightType : std::uint8_t {
	PointLight,
	AreaLight,
	Infinite
};

struct S_VisibilityTester;
/**
 * http://www.pbr-book.org/3ed-2018/Light_Sources/Light_Interface.html
 */
class RENDERER_API_EXPORT I_RayLight {
public:
	I_RayLight(LightType type)
		: m_Type(type)
	{
	}
	virtual ~I_RayLight() = default;
	/**
	 * Samples incident radiance at the intersection given by @param intersection from light.
	 *
	 * @param intersection surface interaction from where we want to sample light
	 * @param vis visibility tester that holds info about shadow ray
	 */
	[[nodiscard]] virtual Colours::T_Colour SampleLi(const C_RayIntersection& intersection, I_Sampler& rnd, S_VisibilityTester& vis, float* pdf) const = 0;

	/**
	 * @brief Returns radiance emitted in a direction of the ray. Used in case ray intersects the lights geometry.
	 * @param point
	 * @param normal
	 * @param uv
	 * @param w
	 * @return
	 */
	[[nodiscard]] virtual Colours::T_Colour Lo(const glm::vec3& point, const glm::vec3& normal, const glm::vec2& uv, const glm::vec3& w) const = 0;
	/**
	 * Returns emitted radiance at a surface point intersected by a ray.
	 *
	 * @TODO it is missing the ray
	 */
	[[nodiscard]] virtual Colours::T_Colour Le() const = 0;
	/**
	 * Returns true for delta lights such as point light
	 */
	bool IsDeltaLight() const { return m_Type == LightType::PointLight; }
	/**
	 *
	 */
	virtual float Pdf_Li(const glm::vec3& wi) const = 0;

	LightType GetType() const { return m_Type; }

protected:
	LightType m_Type;
};
} // namespace GLEngine::Renderer::RayTracing