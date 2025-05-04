#pragma once

#include <AI/BT/IBehavioralNode.h>


namespace GLEngine::AI {
/**
 * @brief This class should not hold the state. State is stored in context.
 */
template <class Context> class BehavioralNode : public I_BehavioralNode {
public:
	using T_Context = Context;
	enum class NodeActivity : std::uint8_t
	{
		/**
		 * @brief only gets update when going down through it, or when child finishes
		 */
		Passive,
		/**
		 * @brief Gets update every time
		 */
		Active,
	};
	explicit BehavioralNode(const T_BTNodeID id)
		: I_BehavioralNode(id)
	{
	}
	UpdateResult		 Update(UpdateConfig&) override;
	void				 OnFinished(UpdateConfig& updateCfg) override {}
	virtual NodeActivity GetNodeActivity() const = 0;
	unsigned int		 GetContextSize() const final { return sizeof(T_Context); }

protected:
	/**
	 * @brief	Here goes the functionality of the node
	 *			Called from the I_BehavioralNode::Update
	 * @todo	Should return if going down, finishing etc.
	 */
	virtual UpdateResult Update(Context* ctx) = 0;
};
} // namespace GLEngine::AI
#include <AI/BT/IBehavioralNode.inl>