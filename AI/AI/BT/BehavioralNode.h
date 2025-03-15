#pragma once

#include <AI/BT/BehaviourTreeExecutor.h>
#include <AI/BT/BTUtils.h>

namespace GLEngine::AI {

struct UpdateConfig {
	C_BehaviourTreeExecutor& Executor;
	float deltaT;
	T_BTNodeInstanceID		 nodeInstanceID;
	// executor allowing to start new nodes
	// also getter for the context
};

class I_BehavioralNode {
public:
	I_BehavioralNode(T_BTNodeID id);
	virtual ~I_BehavioralNode() = default;
	void AddChild(T_BTNodeID);

	enum class UpdateResult {
		Success,
		Failure,
		Finished
	};
	/**
	 * @brief
	 * @todo Gonna cast the context to the correct type
	 */
	virtual UpdateResult Update(UpdateConfig&) = 0;
	virtual void OnFinished(UpdateConfig& updateCfg) = 0;
};
/**
 * @brief This class should not hold the state. State is stored in context.
 */
template<class Context>
class BehavioralNode : public I_BehavioralNode {
public:
	enum class NodeActivity : std::uint8_t {
		/**
		 * @brief only gets update when going down through it, or when child finishes
		 */
		Passive,
		/**
		 * @brief Gets update every time
		 */
		Active,
	};
	UpdateResult Update(UpdateConfig&) override
	{
		// I will get the context from executor
		void* ctx = nullptr;
		return Update(static_cast<Context*>(ctx));
	}
	/**
	 * @brief	Called on start of the node. Should allocate the context
	 * @return	true if node is able to allocate
	 */
	bool OnInit();
	bool CanFinish() const; // true if everything is finished and we can return to the parent
	void				 OnFinished(UpdateConfig& updateCfg) override {
		Context* ctx = updateCfg.Executor.GetContext<Context>(updateCfg.nodeInstanceID);

	}
	virtual NodeActivity GetNodeActivity() const = 0;
protected:
	/**
	 * @brief	Here goes the functionality of the node
	 *			Called from the I_BehavioralNode::Update
	 * @todo	Should return if going down, finishing etc.
	 */
	virtual UpdateResult Update(Context* ctx) = 0;
};


/**
 * Nodes I would like to try:
 * Control flow:
 *	- Branch
 *	- Switch
 *	- Parallel
 *	- Include
 * Functions:
 *	- Go to
 *	- Action
 *	- Wait


 * I need to agree on lifecycle:
 *  - How am I going to start new nodes
 *  - Inside the update or after?
 *  - Who is responsible to deallocate?
 *		* May depend on how the first is executed
 */
struct C_BranchNodeBTCtx;
class C_BranchNodeBT : public BehavioralNode<C_BranchNodeBTCtx> {
public:
	NodeActivity GetNodeActivity() const override { return NodeActivity::Passive; }


private:
	T_BTNodeID m_Child1;
	T_BTNodeID m_Child2;
	// condition
};
struct C_BranchNodeBTCtx {
	T_BTNodeInstanceID activeChild;
};

struct C_ActionNodeBTCtx;
class C_ActionNodeBT : public BehavioralNode<C_ActionNodeBTCtx> {
public:
	NodeActivity GetNodeActivity() const override { return NodeActivity::Passive; } // no child so does not matter


private:
	std::string action;
};
struct C_ActionNodeBTCtx {
	T_BTNodeInstanceID activeChild;
};
} // namespace GLEngine::AI