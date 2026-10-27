#pragma once

#include <GUI/GUIApi.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace GLEngine::GUI::Input {

// Draws translate/rotate/scale controls, converting the quat rotation to/from Euler degrees for
// display only. Returns true when any field changed.
GUI_API_EXPORT bool DrawTransform(glm::vec3& translation, glm::quat& rotation, glm::vec3& scale);

} // namespace GLEngine::GUI::Input
