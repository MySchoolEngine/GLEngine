#pragma once

#include <AI/AIApi.h>
#include <AI/BT/BTUtils.h>
#include <AI/BT/BehavioralTree.h>
#include <AI/BT/IBehavioralNode.h>

namespace GLEngine::AI {
class C_BehavioralTree;
/**
 * @brief
 * @todo For now, we only support one tree. This can be changed in the future for include node.
 *
 * We need to get parent of current node in order to inform parent about the finish of current node.
 */
class AI_API_EXPORT C_BehaviourTreeExecutor {
public:
	explicit C_BehaviourTreeExecutor(C_BehavioralTree* tree);
	template <class Context> T_BTNodeInstanceID StartNewInstance(const T_BTNodeID nodeID);
	void*										GetContext(const T_BTNodeInstanceID nodeInstanceID);
	const void*									GetContext(const T_BTNodeInstanceID nodeInstanceID) const;

	void Update(float deltaT);

private:
	C_BehavioralTree*				m_Tree;
	std::vector<T_BTNodeInstanceID> m_ActiveNodes;
	std::vector<void*>				m_Contexts; // very bad idea
};
template <class Context> T_BTNodeInstanceID C_BehaviourTreeExecutor::StartNewInstance(const T_BTNodeID nodeID)
{
	const auto* node	   = m_Tree->GetNode(nodeID);
	void*		contextBuf = new std::uint8_t[node->GetContextSize()];
	auto*		context	   = new (contextBuf) Context();
	m_Contexts.emplace_back(contextBuf);
	const T_BTNodeInstanceID newId = static_cast<T_BTNodeInstanceID>(m_Contexts.size()) - 1;
	m_ActiveNodes.emplace_back(newId);
	return newId;
}
} // namespace GLEngine::AI