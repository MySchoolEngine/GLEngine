#include <EntityStdafx.h>

#include <Entity/Systems/SystemScheduler.h>

namespace GLEngine::Entity {

//=================================================================================
void SystemScheduler::AddSystem(E_SchedulerStage stage, T_System system)
{
	m_Stages[static_cast<std::size_t>(stage)].push_back(std::move(system));
}

//=================================================================================
void SystemScheduler::Run(C_World& world)
{
	for (auto& stage : m_Stages)
	{
		for (auto& system : stage)
		{
			system(world);
		}
	}
}

} // namespace GLEngine::Entity
