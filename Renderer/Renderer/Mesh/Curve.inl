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
C_BezierCurveInterpolation<CurveT>::C_BezierCurveInterpolation(const CurveT& curve)
	: m_Curve(curve)
{
}

//=================================================================================
template <class CurveT> typename CurveT::T_PointT C_BezierCurveInterpolation<CurveT>::GetPointInTime(float t) const
{
	if (t >= 1.0f) // case of t>=1.0
	{
		return m_Curve.GetControlPoint(m_Curve.GetNumControlPoints() - 1);
	}
	const auto								 ncurves  = 1 + (m_Curve.GetNumControlPoints() - 4) / 3;
	const auto								 denomT	  = t * ncurves;
	const auto								 curve	  = static_cast<std::size_t>(denomT);
	const auto								 tInCurve = denomT - curve;
	std::array<typename CurveT::T_PointT, 4> pts;
	pts[0] = m_Curve.GetControlPoint(curve * 3);
	pts[1] = m_Curve.GetControlPoint(curve * 3 + 1);
	pts[2] = m_Curve.GetControlPoint(curve * 3 + 2);
	pts[3] = m_Curve.GetControlPoint(curve * 3 + 3);

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
