#pragma once

#include <AI/AIApi.h>
#include <AI/BT/BehavioralNode.h>

namespace GLEngine::AI {
struct C_ActionNodeBTCtx;
class AI_API_EXPORT C_ActionNodeBT final : public BehavioralNode<C_ActionNodeBTCtx> {
public:
	explicit C_ActionNodeBT(const T_BTNodeID id)
		: BehavioralNode(id)
	{
	}

	[[nodiscard]] bool AddChild(const T_BTNodeID nodeID, const unsigned int index) override { return false; }
	[[nodiscard]] bool IsValid() const override;
	//==================================================
	// Execution
	//==================================================
	NodeActivity	   GetNodeActivity() const override;
	[[nodiscard]] bool OnInit() override;
	[[nodiscard]] bool CanFinish() const override;
	//==================================================
	// Node metrics
	//==================================================
	[[nodiscard]] unsigned int GetNumChildSlot() const override { return 0; }

protected:
	UpdateResult Update(C_ActionNodeBTCtx* ctx) override;

private:
	std::string action;
};
struct AI_API_EXPORT C_ActionNodeBTCtx{};

} // namespace GLEngine::AI