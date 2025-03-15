#pragma once

#include <AI/BT/BTUtils.h>


namespace GLEngine::AI {
class I_BehavioralNode;

class C_BehavioralTree {
public:
	I_BehavioralNode* GetNode(T_BTNodeID nodeID);
};
}