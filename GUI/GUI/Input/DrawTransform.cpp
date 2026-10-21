#include <GUIStdafx.h>

#include <GUI/Input/DrawTransform.h>

#include <Entity/Components/CoreComponents.h>

#include <glm/gtc/quaternion.hpp>
#include <glm/trigonometric.hpp>

#include <imgui.h>

namespace GLEngine::GUI::Input {

//=================================================================================
bool DrawTransform(GLEngine::Entity::S_TransformComponent& transform)
{
	bool changed = false;

	changed |= ImGui::DragFloat3("Translation", &transform.translation.x, 0.1f);

	glm::vec3 rotationDeg = glm::degrees(glm::eulerAngles(transform.rotation));
	if (ImGui::DragFloat3("Rotation", &rotationDeg.x, 1.0f))
	{
		transform.rotation = glm::quat(glm::radians(rotationDeg));
		changed			   = true;
	}

	changed |= ImGui::DragFloat3("Scale", &transform.scale.x, 0.1f, 0.0001f, FLT_MAX);

	return changed;
}

} // namespace GLEngine::GUI::Input
