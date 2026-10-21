#pragma once

#include <Renderer/RendererApi.h>

#include <entt/entt.hpp>

namespace GLEngine::Renderer::Spike {

struct S_SpikeComponent {
	int value = 0;
};

RENDERER_API_EXPORT void EmplaceSpikeComponent(entt::registry& registry, entt::entity e, int value);

} // namespace GLEngine::Renderer::Spike
