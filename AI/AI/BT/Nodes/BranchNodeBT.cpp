#include <AIStdafx.h>

#include <AI/BT/Nodes/BranchNodeBT.h>

namespace GLEngine::AI {
bool C_BranchNodeBT::AddChild(const T_BTNodeID nodeID, const unsigned int index)
{
	if (index == Branches::Positive)
	{
		m_Child1 = nodeID;
	}
	if (index == Branches::Negative)
	{
		m_Child2 = nodeID;
	}
	return true;
}
} // namespace GLEngine::AI
