#pragma once

#include <Renderer/Mesh/Scene.h>
#include <Renderer/RayCasting/Frame.h>

namespace GLEngine::Renderer {
namespace RayTracing {
class I_RayLight;
}
class RENDERER_API_EXPORT C_RayIntersection {
public:
	C_RayIntersection(); //= default;
	C_RayIntersection(S_Frame&& frame, glm::vec3&& point, Physics::Primitives::S_Ray&& ray);
	~C_RayIntersection(); //= default;

	[[nodiscard]] const S_Frame&					GetFrame() const;
	[[nodiscard]] const Physics::Primitives::S_Ray& GetRay() const;
	[[nodiscard]] const glm::vec3&					GetIntersectionPoint() const;
	[[nodiscard]] const float						GetRayLength() const;

	void									SetMaterial(const MeshData::Material* material);
	[[nodiscard]] const MeshData::Material* GetMaterial() const;

	[[nodiscard]] bool											IsLight() const;
	void														SetLight(const std::shared_ptr<RayTracing::I_RayLight>& light);
	[[nodiscard]] const std::shared_ptr<RayTracing::I_RayLight> GetLight() const;

private:
	S_Frame						m_Frame;
	glm::vec3					m_Point;
	Physics::Primitives::S_Ray	m_Ray;
	const MeshData::Material*	m_Material; // not owning
	std::shared_ptr<RayTracing::I_RayLight> m_Light;
};
} // namespace GLEngine::Renderer
