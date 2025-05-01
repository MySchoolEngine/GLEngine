#pragma once

#include <AI/AIApi.h>
#include <AI/BT/BehavioralNode.h>

namespace GLEngine::AI {
struct C_BranchNodeBTCtx;
class AI_API_EXPORT C_BranchNodeBT : public BehavioralNode<C_BranchNodeBTCtx> {
public:
	explicit C_BranchNodeBT(T_BTNodeID id)
		: BehavioralNode(id)
	{
	}
	~C_BranchNodeBT() override = default;
	NodeActivity	   GetNodeActivity() const override { return NodeActivity::Passive; }
	UpdateResult	   Update(UpdateConfig&) override { return UpdateResult::Finished; } // TODO: implement
	void			   OnFinished(UpdateConfig& updateCfg) override {}
	UpdateResult	   Update(C_BranchNodeBTCtx* ctx) override { return UpdateResult::Finished; }
	[[nodiscard]] bool IsValid() const override { return m_Child1 != s_InvalidNodeID && m_Child2 != s_InvalidNodeID; } // TODO: implement
	//==================================================
	// Execution
	//==================================================
	[[nodiscard]] bool OnInit() override { return false; }
	[[nodiscard]] bool CanFinish() const override { return false; }

private:
	T_BTNodeID m_Child1 = s_InvalidNodeID;
	T_BTNodeID m_Child2 = s_InvalidNodeID;
	// condition
};
struct AI_API_EXPORT C_BranchNodeBTCtx {
	T_BTNodeInstanceID activeChild;
};
} // namespace GLEngine::AI