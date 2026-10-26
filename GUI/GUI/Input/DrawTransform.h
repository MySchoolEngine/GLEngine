#pragma once

#include <GUI/GUIApi.h>

namespace GLEngine::Entity {
struct S_TransformComponent;
}

namespace GLEngine::GUI::Input {

// Draws translate/rotate/scale controls for a S_TransformComponent, converting its quat
// rotation to/from Euler degrees for display only. Returns true when the component changed.
GUI_API_EXPORT bool DrawTransform(GLEngine::Entity::S_TransformComponent& transform);

} // namespace GLEngine::GUI::Input
