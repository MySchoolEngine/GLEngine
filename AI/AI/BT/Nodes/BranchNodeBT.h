#pragma once

#include <AI/AIApi.h>
#include <AI/BT/BehavioralNode.h>


namespace GLEngine::AI {
struct C_BranchNodeBTCtx;
class AI_API_EXPORT C_BranchNodeBT final : public BehavioralNode<C_BranchNodeBTCtx> {
public:
	explicit C_BranchNodeBT(const T_BTNodeID id)
		: BehavioralNode(id)
	{
	}
	~C_BranchNodeBT() override = default;

	enum Branches {
		Positive,
		Negative,
	};
	[[nodiscard]] bool		   AddChild(const T_BTNodeID nodeID, const unsigned int index) override;
	[[nodiscard]] UpdateResult Update(C_BranchNodeBTCtx* ctx) override { return UpdateResult::Finished; }
	[[nodiscard]] bool		   IsValid() const override { return m_Child1 != s_InvalidNodeID && m_Child2 != s_InvalidNodeID; } // TODO: implement
	//==================================================
	// Execution
	//==================================================
	[[nodiscard]] NodeActivity GetNodeActivity() const override { return NodeActivity::Passive; }
	[[nodiscard]] UpdateResult Update(UpdateConfig&) override { return UpdateResult::Finished; } // TODO: implement
	void					   OnFinished(UpdateConfig& updateCfg) override {}
	[[nodiscard]] bool		   OnInit() override { return false; }
	[[nodiscard]] bool		   CanFinish() const override { return false; }
	//==================================================
	// Node metrics
	//==================================================
	[[nodiscard]] unsigned int GetNumChildSlot() const override { return 2; }

private:
	T_BTNodeID m_Child1 = s_InvalidNodeID;
	T_BTNodeID m_Child2 = s_InvalidNodeID;
	// condition
	friend class C_BehavioralTreeFixture;
};
struct AI_API_EXPORT C_BranchNodeBTCtx {
	T_BTNodeInstanceID activeChild = s_InvalidNodeInstanceID;
};
} // namespace GLEngine::AI