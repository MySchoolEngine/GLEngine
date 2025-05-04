#pragma once

#include <AI/BT/BTUtils.h>


namespace GLEngine::AI {
class C_BehaviourTreeExecutor;
struct UpdateConfig {
	C_BehaviourTreeExecutor& Executor;
	float					 deltaT;
	T_BTNodeInstanceID		 nodeInstanceID;
	// executor allowing to start new nodes
	// also getter for the context
};
} // namespace GLEngine::AI