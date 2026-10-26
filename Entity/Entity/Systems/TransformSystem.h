#pragma once

#include <Entity/EntityApi.h>

namespace GLEngine::Entity {

class C_World;

namespace TransformSystem {
ENTITY_API_EXPORT void Update(C_World& world);
}

} // namespace GLEngine::Entity
