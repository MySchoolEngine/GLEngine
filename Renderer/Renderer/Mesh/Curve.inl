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

//=================================================================================
// C_BezierCurveInterpolation
//=================================================================================
template <class CurveT>
C_BezierCurveInterpolation<CurveT>::C_BezierCurveInterpolation(const CurveT& curve, bool looped)
	: m_Curve(curve)
	, m_Looped(looped)
{
}

//=================================================================================
template <class CurveT> typename CurveT::T_PointT C_BezierCurveInterpolation<CurveT>::GetPointInTime(float t) const
{
	if (t >= 1.0f && !m_Looped) // case of t>=1.0
	{
		return m_Curve.GetControlPoint(m_Curve.GetNumControlPoints() - 1);
	}
	const auto curveNumPoints = m_Curve.GetNumControlPoints();
	const auto ncurves		  = 1 + (curveNumPoints - (m_Looped ? 3 : 4)) / 3;
	const auto denomT		  = t * ncurves;
	const auto curve		  = static_cast<std::size_t>(denomT);
	const auto tInCurve		  = denomT - curve;
	const auto firstPoint	  = curve * 3;

	std::array<typename CurveT::T_PointT, 4> pts;
	pts[0] = m_Curve.GetControlPoint(firstPoint % curveNumPoints);
	pts[1] = m_Curve.GetControlPoint((firstPoint + 1) % curveNumPoints);
	pts[2] = m_Curve.GetControlPoint((firstPoint + 2) % curveNumPoints);
	pts[3] = m_Curve.GetControlPoint((firstPoint + 3) % curveNumPoints);

	return evalBezierCurve(pts, tInCurve);
}

//=================================================================================
template <class CurveT> typename CurveT::T_PointT C_BezierCurveInterpolation<CurveT>::evalBezierCurve(const std::array<typename CurveT::T_PointT, 4>& points, const float& t) const
{
	const float b0 = std::pow(1 - t, 3.0f);
	const float b1 = 3.f * t * std::pow(1.f - t, 2.0f);
	const float b2 = 3.f * t * t * (1.0f - t);
	const float b3 = t * t * t;
	return points[0] * b0 + points[1] * b1 + points[2] * b2 + points[3] * b3;
}
} // namespace GLEngine::Renderer
