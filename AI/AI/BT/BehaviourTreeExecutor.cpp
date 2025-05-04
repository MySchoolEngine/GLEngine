#include <AIStdafx.h>

#include <AI/BT/BehaviourTreeExecutor.h>

namespace GLEngine::AI {
C_BehaviourTreeExecutor::C_BehaviourTreeExecutor(C_BehavioralTree* tree)
	: m_Tree(tree)
{
}

//=================================================================================
void C_BehaviourTreeExecutor::Update(float deltaT)
{
	// Remove inactive nodes
	std::vector<T_BTNodeInstanceID> toBeDestructed;
	for (const auto& instanceID : m_ActiveNodes)
	{

	}

	// std::remove()

	for (const auto& instanceID : m_ActiveNodes) {}
}

//=================================================================================
void* C_BehaviourTreeExecutor::GetContext(const T_BTNodeInstanceID nodeInstanceID)
{
	return m_Contexts[nodeInstanceID];
}

//=================================================================================
const void* C_BehaviourTreeExecutor::GetContext(const T_BTNodeInstanceID nodeInstanceID) const
{
	return m_Contexts[nodeInstanceID];
}
} // namespace GLEngine::AI