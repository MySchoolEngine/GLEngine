#pragma once

#include <Renderer/RendererApi.h>

namespace GLEngine::Renderer {
// this class is only responsible for storage of curve data
class RENDERER_API_EXPORT C_Curve {
public:
	using T_PointT = glm::vec3;
	C_Curve();
	void					  AddControlPoint(const std::size_t position, const T_PointT& point);
	void					  SetControlPoint(const std::size_t position, const T_PointT& point);
	void					  RemoveControlPoint(const std::size_t position);
	void					  RemoveControlPoint(const std::set<std::size_t>& points);
	[[nodiscard]] T_PointT	  GetControlPoint(const std::size_t position) const;
	[[nodiscard]] std::size_t GetNumControlPoints() const;

	using T_ControlPointFn = std::function<void(T_PointT&)>;
	void ForEachControlPoint(const T_ControlPointFn& fn);

private:
	std::vector<T_PointT> m_Points;
};

// Gets curve and can return you point on a curve for given parametr t \in [0;1]
template <class pointType> class I_CurveFunction {
public:
	~I_CurveFunction()											  = default;
	[[nodiscard]] virtual pointType GetPointInTime(float t) const = 0;
};

// This class is doing the linear interpolation
template <class CurveT> class C_LinearCurveInterpolation : I_CurveFunction<typename CurveT::T_PointT> {
public:
	C_LinearCurveInterpolation(const CurveT& curve);
	[[nodiscard]] typename CurveT::T_PointT GetPointInTime(float t) const override;

private:
	const CurveT& m_Curve;
};

template <class CurveT> class C_BezierCurveInterpolation : I_CurveFunction<typename CurveT::T_PointT> {
public:
	explicit C_BezierCurveInterpolation(const CurveT& curve)
		: m_Curve(curve)
	{}

	[[nodiscard]] typename CurveT::T_PointT GetPointInTime(float t) const override
	{
		if (t >= 1.0f) // case of t>=1.0
		{
			return m_Curve.GetControlPoint(m_Curve.GetNumControlPoints() - 1);
		}
		const auto ncurves = 1 + (m_Curve.GetNumControlPoints() - 4) / 3;
		const auto denomT  = t * ncurves;
		const auto curve   = static_cast<std::size_t>(denomT);
		const auto tInCurve = denomT - curve;
		std::array<typename CurveT::T_PointT, 4> pts;
		pts[0] = m_Curve.GetControlPoint(curve * 3);
		pts[1] = m_Curve.GetControlPoint(curve * 3 + 1);
		pts[2] = m_Curve.GetControlPoint(curve * 3 + 2);
		pts[3] = m_Curve.GetControlPoint(curve * 3 + 3);

		return evalBezierCurve(pts, tInCurve);
	}

private:
	typename CurveT::T_PointT evalBezierCurve(const std::array<typename CurveT::T_PointT, 4>& points, const float& t) const
	{
		const float b0 = std::pow(1 - t, 3.0f);
		const float b1 = 3.f * t * std::pow(1.f - t, 2.0f);
		const float b2 = 3.f * t * t * (1.0f - t);
		const float b3 = t * t * t;
		return points[0] * b0 + points[1] * b1 + points[2] * b2 + points[3] * b3;
	}
	const CurveT& m_Curve;
};


} // namespace GLEngine::Renderer

#include <Renderer/Mesh/Curve.inl>