#include <EntityTestStdafx.h>

#include <Renderer/Experimental/SpikeComponent.h>

#include <Entity/DllBoundarySpike.h>

#include <entt/entt.hpp>

namespace GLEngine::Entity {

TEST(DllBoundarySpike, ComponentDefinedInRendererRoundTripsThroughEntityOwnedRegistry)
{
	auto& registry = Spike::GetRegistry();
	registry.clear();

	const auto e = registry.create();
	// emplace<S_SpikeComponent> is instantiated inside Renderer.dll here.
	Renderer::Spike::EmplaceSpikeComponent(registry, e, 42);

	// all_of<T>/get<T>/view<T> below are instantiated inside EntityTest.exe,
	// a different compiled binary from the one that did the emplace above.
	ASSERT_TRUE(registry.all_of<Renderer::Spike::S_SpikeComponent>(e));
	EXPECT_EQ(registry.get<Renderer::Spike::S_SpikeComponent>(e).value, 42);

	int viewCount = 0;
	for (auto&& [entity, comp] : registry.view<Renderer::Spike::S_SpikeComponent>().each())
	{
		EXPECT_EQ(comp.value, 42);
		++viewCount;
	}
	EXPECT_EQ(viewCount, 1);
}

} // namespace GLEngine::Entity
