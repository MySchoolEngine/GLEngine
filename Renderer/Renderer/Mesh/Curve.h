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
template <class CurveT> class C_LinearCurveInterpolation : public I_CurveFunction<typename CurveT::T_PointT> {
public:
	C_LinearCurveInterpolation(const CurveT& curve);
	[[nodiscard]] typename CurveT::T_PointT GetPointInTime(float t) const override;

private:
	const CurveT& m_Curve;
};

template <class CurveT> class C_BezierCurveInterpolation : public I_CurveFunction<typename CurveT::T_PointT> {
public:
	explicit C_BezierCurveInterpolation(const CurveT& curve, bool looped = false);

	[[nodiscard]] typename CurveT::T_PointT GetPointInTime(float t) const override;

protected:
	typename CurveT::T_PointT evalBezierCurve(const std::array<typename CurveT::T_PointT, 4>& points, const float& t) const;
	typename CurveT::T_PointT derviBezierCurve(const std::array<typename CurveT::T_PointT, 4>& points, const float& t) const;
	const CurveT&			  m_Curve;
	bool					  m_Looped;
};

template <class CurveT> class C_SmoothBezierCurveInterpolation : public C_BezierCurveInterpolation<CurveT> {
public:
	using T_Base = C_BezierCurveInterpolation<CurveT>;
	explicit C_SmoothBezierCurveInterpolation(const CurveT& curve, bool looped = false)
		: C_BezierCurveInterpolation<CurveT>(curve, looped)
	{
	}

	[[nodiscard]] typename CurveT::T_PointT GetPointInTime(float t) const override
	{
		const auto curveNumPoints = T_Base::m_Curve.GetNumControlPoints();

		if (t >= 1.0f) // case of t>=1.0
		{
			return T_Base::m_Curve.GetControlPoint(T_Base::m_Curve.GetNumControlPoints() - 1);
		}
		std::array<typename CurveT::T_PointT, 4> pts;
		if (curveNumPoints <= 4)
		{
			for (int i = 0; i < 4; ++i)
			{
				if (i < curveNumPoints)
				{
					pts[i] = T_Base::m_Curve.GetControlPoint(i);
				}
				else
				{
					pts[i] = T_Base::m_Curve.GetControlPoint(curveNumPoints - 1);
				}
			}

			return T_Base::evalBezierCurve(pts, t);
		}
		const auto ncurves	  = 1 + (curveNumPoints - 3) / 2;
		const auto denomT	  = t * ncurves;
		const auto curve	  = static_cast<std::size_t>(denomT);
		const auto tInCurve	  = denomT - curve;
		const auto firstPoint = curve * 3;

		if (curve == 0)
		{
			pts[0] = T_Base::m_Curve.GetControlPoint(0);
			pts[1] = T_Base::m_Curve.GetControlPoint(1);
			pts[2] = T_Base::m_Curve.GetControlPoint(2);
			pts[3] = T_Base::m_Curve.GetControlPoint(3);
			pts[3] = (pts[2] + pts[3]) / 2.f;
		}
		else if (curve == ncurves - 1)
		{
			const auto usedPoints = 3 + 2 * (curve - 1);
			pts[0]				  = (T_Base::m_Curve.GetControlPoint(usedPoints - 1) + T_Base::m_Curve.GetControlPoint(usedPoints)) / 2.f;
			for (int i = 1; i < 4; ++i)
			{
				const auto index = usedPoints - 1 + i;
				pts[i]			 = T_Base::m_Curve.GetControlPoint((index < curveNumPoints) ? index : curveNumPoints - 1);
			}
		}
		else
		{
		}

		return T_Base::evalBezierCurve(pts, tInCurve);
	}
};

} // namespace GLEngine::Renderer

#include <Renderer/Mesh/Curve.inl>