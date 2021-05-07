#include <RendererStdafx.h>

#include <Renderer/Mesh/Curve.h>


namespace GLEngine::Renderer {

//=================================================================================
C_Curve::C_Curve()
{
}

//=================================================================================
void C_Curve::AddControlPoint(const std::size_t position, const glm::vec3& point)
{
	auto it = m_Points.begin();
	std::advance(it, position);
	m_Points.insert(it, point);
}

//=================================================================================
void C_Curve::ForEachControlPoint(const T_ControlPointFn& fn)
{
	std::for_each(m_Points.begin(), m_Points.end(), fn);
}

//=================================================================================
void C_Curve::SetControlPoint(const std::size_t position, const glm::vec3& point)
{
	auto it = m_Points.begin();
	std::advance(it, position);
	*it = point;
}

//=================================================================================
void C_Curve::RemoveControlPoint(const std::size_t position)
{
	auto it = m_Points.begin();
	std::advance(it, position);
	m_Points.erase(it);
}

//=================================================================================
void C_Curve::RemoveControlPoint(const std::set<std::size_t>& points)
{

}

//=================================================================================
glm::vec3 C_Curve::GetControlPoint(const std::size_t position)
{
	auto it = m_Points.begin();
	std::advance(it, position);
	return *it;
}

//=================================================================================
std::size_t C_Curve::GetNumControlPoints() const
{
	return m_Points.size();
}

} // namespace GLEngine::Renderer