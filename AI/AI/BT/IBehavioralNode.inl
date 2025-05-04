#pragma once

#include <AI/BT/BehaviourTreeExecutor.h>

namespace GLEngine::AI {
template <class Context>
I_BehavioralNode::UpdateResult BehavioralNode<Context>::Update(UpdateConfig& updateCfg)
{
	// I will get the context from executor
	auto* ctx = updateCfg.Executor.GetContext(updateCfg.nodeInstanceID);
	return Update(static_cast<Context*>(ctx));
}
} // namespace GLEngine::AI