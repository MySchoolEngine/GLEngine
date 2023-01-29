#include <RendererStdafx.h>

#include <Renderer/Mesh/Scene.h>
#include <Renderer/RayCasting/RayIntersection.h>

namespace GLEngine::Renderer {
//=================================================================================
C_RayIntersection::C_RayIntersection(S_Frame&& frame, glm::vec3&& point, Physics::Primitives::S_Ray&& ray)
	: m_Frame(frame)
	, m_Point(point)
	, m_Ray(ray)
	, m_Material(nullptr)
	, m_Light(nullptr)
	, m_UV({0.f, 0.f})
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
	return m_RayLength;
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

//=================================================================================
void C_RayIntersection::TransformRayAndPoint(const glm::mat4& mat)
{
	m_Point = mat * glm::vec4(m_Point, 1.f);
	m_Ray.origin = mat * glm::vec4(m_Ray.origin, 1.f);
}

} // namespace GLEngine::Renderer
