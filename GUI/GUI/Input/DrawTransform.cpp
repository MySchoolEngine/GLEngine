#include <GUIStdafx.h>

#include <GUI/Input/DrawTransform.h>

#include <glm/gtc/quaternion.hpp>
#include <glm/trigonometric.hpp>

#include <imgui.h>

namespace GLEngine::GUI::Input {

//=================================================================================
bool DrawTransform(glm::vec3& translation, glm::quat& rotation, glm::vec3& scale)
{
	bool changed = false;

	changed |= ImGui::DragFloat3("Translation", &translation.x, 0.1f);

	glm::vec3 rotationDeg = glm::degrees(glm::eulerAngles(rotation));
	if (ImGui::DragFloat3("Rotation", &rotationDeg.x, 1.0f))
	{
		rotation = glm::quat(glm::radians(rotationDeg));
		changed	 = true;
	}

	changed |= ImGui::DragFloat3("Scale", &scale.x, 0.1f, 0.0001f, FLT_MAX);

	return changed;
}

} // namespace GLEngine::GUI::Input
