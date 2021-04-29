#pragma once

namespace GLEngine::Core {
class I_Input;
}

namespace GLEngine::Renderer {
class I_DebugDraw;
class I_CameraComponent;
} // namespace GLEngine::Renderer

namespace GLEngine::Editor {
class C_Gizmo {
public:
	C_Gizmo(const glm::vec3& position);

	glm::vec3 GetPosition() const;
	void	  SetPosition(const glm::vec3& position);

	void OnUpdate(const Core::I_Input& input, const Renderer::I_CameraComponent& camera);
	void Draw(Renderer::I_DebugDraw& dd) const;

	[[nodiscard]] bool IsMouseOverGizmo() const;

private:
	enum class E_Direction
	{
		Right,
		Up,
		Forward
	};
	glm::vec3				   m_Position;
	std::optional<E_Direction> m_MouseOverDirection;
};
} // namespace GLEngine::Editor
