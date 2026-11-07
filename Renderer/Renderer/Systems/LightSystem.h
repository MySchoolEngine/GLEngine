#pragma once

#include <Renderer/RendererApi.h>

namespace GLEngine::Entity {
class C_World;
}

namespace GLEngine::Renderer::LightSystem {
RENDERER_API_EXPORT void Update(Entity::C_World& world);
}
