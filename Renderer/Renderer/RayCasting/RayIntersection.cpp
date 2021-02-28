#include <RendererStdafx.h>

#include <Renderer/RayCasting/RayIntersection.h>

namespace GLEngine::Renderer {
//=================================================================================
C_RayIntersection::C_RayIntersection(S_Frame&& frame, glm::vec3&& point, Physics::Primitives::S_Ray&& ray)
	: m_Frame(frame)
	, m_Point(point)
	, m_Ray(ray)
	, m_Material(nullptr)
	, m_Light(nullptr)
{
}

//=================================================================================
C_RayIntersection::C_RayIntersection() = default;

//=================================================================================
C_RayIntersection::~C_RayIntersection() = default;

//=================================================================================
bool C_RayIntersection::IsLight() const
{
	return m_Light != nullptr;
}

//=================================================================================
const GLEngine::Renderer::MeshData::Material* C_RayIntersection::GetMaterial() const
{
	return m_Material;
}

//=================================================================================
void C_RayIntersection::SetMaterial(const MeshData::Material* material)
{
	m_Material = material;
}

//=================================================================================
const float C_RayIntersection::GetRayLength() const
{
	return glm::distance(m_Point, m_Ray.origin);
}

//=================================================================================
const glm::vec3& C_RayIntersection::GetIntersectionPoint() const
{
	return m_Point;
}

//=================================================================================
const GLEngine::Physics::Primitives::S_Ray& C_RayIntersection::GetRay() const
{
	return m_Ray;
}

//=================================================================================
const GLEngine::Renderer::S_Frame& C_RayIntersection::GetFrame() const
{
	return m_Frame;
}

//=================================================================================
const std::shared_ptr<GLEngine::Renderer::RayTracing::I_RayLight> C_RayIntersection::GetLight() const
{
	return m_Light;
}

//=================================================================================
void C_RayIntersection::SetLight(const std::shared_ptr<RayTracing::I_RayLight>& light)
{
	m_Light = light;
}

} // namespace GLEngine::Renderer
