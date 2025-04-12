#include <AIStdafx.h>

#include <AI/BT/BehavioralTree.h>

namespace GLEngine::AI {

//=================================================================================
bool C_BehavioralTree::AddChild(T_BTNodeID parent, T_BTNodeID child)
{
	return false;
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
