#pragma once

#include <AI/BT/BTUtils.h>

namespace GLEngine::AI {
class C_BehavioralTree;
/**
 * @brief
 * @todo For now we only support one tree. This can be changed in the future for include node.
 *
 * We need to get parent of current node in order to inform parent about the finish of current node.
 */
class C_BehaviourTreeExecutor {
public:
	C_BehaviourTreeExecutor(C_BehavioralTree* tree);
	template <class Context> Context* StartNewInstance(T_BTNodeID nodeID);
	template <class Context> Context* GetContext(T_BTNodeInstanceID nodeInstanceID);

	void Update(float deltaT);

private:
	C_BehavioralTree* m_Tree;
	std::vector<T_BTNodeInstanceID> m_ActiveNodes;
};
} // namespace GLEngine::AI