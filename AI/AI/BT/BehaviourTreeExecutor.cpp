#include <AIStdafx.h>

#include <AI/BT/BehaviourTreeExecutor.h>


namespace GLEngine::AI {
//=================================================================================
void C_BehaviourTreeExecutor::Update(float deltaT)
{
	std::vector<T_BTNodeInstanceID> toBeDestructed;
	for (const auto& instanceID : m_ActiveNodes)
	{
		
	}

	//std::remove()
}
} // namespace GLEngine::AI