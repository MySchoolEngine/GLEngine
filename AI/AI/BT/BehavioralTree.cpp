#include <AIStdafx.h>

#include "IBehavioralNode.h"
#include <AI/BT/BehavioralTree.h>

namespace GLEngine::AI {

//=================================================================================
bool C_BehavioralTree::AddChild(T_BTNodeID parent, T_BTNodeID child, const unsigned int index)
{
	auto* parentNode = GetNode(parent);
	if (parentNode->GetNumChildSlot() == 0)
	{
		CORE_LOG(E_Level::Error, E_Context::AI, "This node is leaf.");
		return false;
	}
	if (index >= parentNode->GetNumChildSlot())
	{
		CORE_LOG(E_Level::Error, E_Context::AI, "This node does not have so many slots.");
		return false;
	}
	return parentNode->AddChild(child, index);
}

//=================================================================================
I_BehavioralNode* C_BehavioralTree::GetNode(const T_BTNodeID nodeID)
{
	return m_Nodes[nodeID];
}

//=================================================================================
bool C_BehavioralTree::LoadFromResource(const std::string& resourcePath)
{
	return false;
}

//=================================================================================
std::unique_ptr<Core::I_ResourceLoader> C_BehavioralTree::GetLoader()
{
	return std::make_unique<BehavioralTreeLoader>();
}
} // namespace GLEngine::AI
