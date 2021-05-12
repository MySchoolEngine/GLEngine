#pragma once

namespace GLEngine::Renderer {

//=================================================================================
// C_CurveFunction
//=================================================================================
template <class CurveT>
C_LinearCurveInterpolation<CurveT>::C_LinearCurveInterpolation(const CurveT& curve)
	: m_Curve(curve)
{
}

//=================================================================================
template <class CurveT> typename CurveT::T_PointT C_LinearCurveInterpolation<CurveT>::GetPointInTime(float t) const
{
	const auto curveLenght		 = m_Curve.GetNumControlPoints();
	const auto progress			 = t * (curveLenght - 1);
	const auto lineSegment		 = static_cast<int>(std::floor(progress));
	const auto inSegmentProgress = progress - lineSegment;
	if (lineSegment == curveLenght - 1)
	{
		return m_Curve.GetControlPoint(lineSegment);
	}
	return glm::mix(m_Curve.GetControlPoint(lineSegment), m_Curve.GetControlPoint(lineSegment + 1), inSegmentProgress);
}
} // namespace GLEngine::Renderer
