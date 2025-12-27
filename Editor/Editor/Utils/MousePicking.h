#pragma once

#include <Renderer/Viewport.h>

namespace GLEngine::Core {
class I_Input;
}
namespace GLEngine::Renderer {
class I_CameraComponent;
} // namespace GLEngine::Renderer

namespace GLEngine::Editor {

/**
 * This class facilitate simple mouse interaction. It is meant as a helper not a long-lived  object.
 * You feed it all the possible interactions, and then you call SelectInteraction to evaluate the interactions
 */
class C_MousePickingHelper {
public:
	C_MousePickingHelper(const Core::I_Input& input, const Renderer::I_CameraComponent& camera, const Renderer::C_Viewport& viewPort);

	// will be called when the interaction will be selected
	using T_Callback = std::function<void()>;
	void LineSegment(const glm::vec3& a, const glm::vec3& b, const T_Callback& cb);
	void Point(const glm::vec3& point, const T_Callback& cb);

	// return true if any interaction have been selected
	bool SelectInteraction();

private:
	const Core::I_Input&			   m_Input;
	const Renderer::I_CameraComponent& m_Camera;
	const Renderer::C_Viewport		   m_Viewport;

	// some structure with distance, depth and interactionID
	struct Interaction {
		Interaction(float _distance, float _depth, std::uint8_t _priority, T_Callback _cb)
			: distance(_distance)
			, depth(_depth)
			, priority(_priority)
			, cb(std::move(_cb))
		{
		}
		float		 distance;
		float		 depth;
		std::uint8_t priority; //< higher priority is preferred over the lower one
		T_Callback	 cb;
	};
	std::vector<Interaction> m_Interactions;

	static constexpr float s_MaxDistanceToInteraction = 35.f; //< in pixels
};
} // namespace GLEngine::Editor