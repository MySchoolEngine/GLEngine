#include <AIStdafx.h>

#include <AI/BT/Nodes/ActionNodeBT.h>


namespace GLEngine::AI {

//=================================================================================
BehavioralNode<C_ActionNodeBTCtx>::NodeActivity C_ActionNodeBT::GetNodeActivity() const
{
	return NodeActivity::Passive; // no child so does not matter
}

//=================================================================================
bool C_ActionNodeBT::IsValid() const
{
	return !action.empty();
}

//=================================================================================
bool C_ActionNodeBT::OnInit()
{
	return true;
}

//=================================================================================
bool C_ActionNodeBT::CanFinish() const
{
	return true; // TODO: I have no idea
}

//=================================================================================
I_BehavioralNode::UpdateResult C_ActionNodeBT::Update(C_ActionNodeBTCtx* ctx)
{
	CORE_LOG(E_Level::Info, E_Context::AI, "Action node: {}", action);
	return UpdateResult::Finished;
}
} // namespace GLEngine::AI