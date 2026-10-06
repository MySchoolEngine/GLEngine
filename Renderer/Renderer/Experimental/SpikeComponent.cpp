#include <RendererStdafx.h>

#include <Renderer/Experimental/SpikeComponent.h>

namespace GLEngine::Renderer::Spike {

void EmplaceSpikeComponent(entt::registry& registry, entt::entity e, int value)
{
	registry.emplace<S_SpikeComponent>(e, value);
}

} // namespace GLEngine::Renderer::Spike
