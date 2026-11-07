#pragma once

#include <Entity/EntityApi.h>

#include <array>
#include <cstdint>
#include <functional>
#include <vector>

namespace GLEngine::Entity {

class C_World;

enum class E_SchedulerStage : std::uint8_t {
	PreUpdate,
	Update,
	PostUpdate,
	Count,
};

class ENTITY_API_EXPORT SystemScheduler {
public:
	using T_System = std::function<void(C_World&)>;

	// Limitations, not enforced: `stage` is not bounds-checked (must be < Count, or
	// indexing m_Stages is UB), and this must not be called from inside Run() below - Run
	// range-for's each stage's vector, and this push_back's into the same vector, so registering
	// a system while one is currently running invalidates that iteration.
	void AddSystem(E_SchedulerStage stage, T_System system);
	void Run(C_World& world);

private:
	std::array<std::vector<T_System>, static_cast<std::size_t>(E_SchedulerStage::Count)> m_Stages;
};

} // namespace GLEngine::Entity
