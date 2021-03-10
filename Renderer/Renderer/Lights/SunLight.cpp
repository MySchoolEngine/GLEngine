#include <RendererStdAfx.h>

#include <Renderer/Lights/SunLight.h>

namespace GLEngine::Renderer {

//=================================================================================
C_SunLight::C_SunLight(std::shared_ptr<Entity::I_Entity> owner)
	: I_Light(owner)
	, m_SunX(0.437f, -1.f, 1.f, "Sun X")
	, m_SunY(0.056f, -1.f, 1.f, "Sun Y")
	, m_SunZ(-0.347f, -1.f, 1.f, "Sun Z")
	, m_SunColor("Sun color", glm::vec3(1.f))
	, m_AsymetricFactor(0.95f, 0.0f, 1.f, "Asymmetric factor")
	, m_SunDiscMultiplier(1.f, 1.0f, 20.f, "Disc multiplier")
{

}

//=================================================================================
C_SunLight::~C_SunLight() = default;

//=================================================================================
bool C_SunLight::HasDebugDrawGUI() const
{
	return true;
}

//=================================================================================
std::string_view C_SunLight::GetDebugComponentName() const
{
	return "Sun";
}

//=================================================================================
void C_SunLight::DebugDrawGUI()
{
	m_SunX.Draw();
	m_SunY.Draw();
	m_SunZ.Draw();
	m_SunColor.Draw();
	m_AsymetricFactor.Draw();
	m_SunDiscMultiplier.Draw();
}

//=================================================================================
void C_SunLight::DebugDraw() const
{
	// C_DebugDraw::Instance().DrawPoint({ m_SunX.GetValue(), m_SunY.GetValue(), m_SunZ.GetValue() }, { 1.f, 1.f, 0.f });
	// C_DebugDraw::Instance().DrawLine({ 0.f, 0.f, 0.f }, { m_SunX.GetValue(), m_SunY.GetValue(), m_SunZ.GetValue() }, { 1.f, 1.f, 0.f });
}

//=================================================================================
glm::vec3 C_SunLight::GetSunDirection() const
{
	return { m_SunX.GetValue(), m_SunY.GetValue(), m_SunZ.GetValue() };
}

//=================================================================================
glm::vec3 C_SunLight::GetSunColor() const
{
	return m_SunColor.GetValue();
}

//=================================================================================
float C_SunLight::SunDiscMultiplier() const
{
	return m_SunDiscMultiplier;
}

//=================================================================================
float C_SunLight::AtmosphereAsymetricFactor() const
{
	return m_AsymetricFactor;
}

//=================================================================================
Physics::Primitives::C_Frustum C_SunLight::GetShadingFrustum() const
{
	return Physics::Primitives::C_Frustum(glm::vec3(0.f), glm::vec3(0.f), glm::vec3(0.f), 0, 0, 0, 0);
}

//=================================================================================
Physics::Primitives::S_AABB C_SunLight::GetAABB() const
{
	return Physics::Primitives::S_AABB();
}

}
