#pragma once

#include <Renderer/RendererApi.h>

namespace GLEngine::Renderer {
// this class is only responsible for storage of curve data
class RENDERER_API_EXPORT C_Curve {
public:
	C_Curve();
	void					AddControlPoint(const std::size_t position, const glm::vec3& point);
	void					SetControlPoint(const std::size_t position, const glm::vec3& point);
	void					RemoveControlPoint(const std::size_t position);
	[[nodiscard]] glm::vec3 GetControlPoint(const std::size_t position);

	using T_ControlPointFn = std::function<void(glm::vec3)>;
	void ForEachControlPoint(const T_ControlPointFn& fn);

private:
	std::vector<glm::vec3> m_Points;
};
} // namespace GLEngine::Renderer
