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

template <class pointType> class I_CurveFunction {
public:
	~I_CurveFunction()											  = default;
	[[nodiscard]] virtual pointType GetPointInTime(float t) const = 0;
};

// Gets curve and can return you point on a curve for given parametr t \in [0;1]
// This class is doing the linear interpolation
template <class CurveT> class C_LinearCurveInterpolation : I_CurveFunction<typename CurveT::T_PointT> {
public:
	C_LinearCurveInterpolation(const CurveT& curve);
	[[nodiscard]] typename CurveT::T_PointT GetPointInTime(float t) const override;

private:
	const CurveT& m_Curve;
};

template class C_LinearCurveInterpolation<C_Curve>;

} // namespace GLEngine::Renderer

#include <Renderer/Mesh/Curve.inl>