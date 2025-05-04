#pragma once

#include <AI/AIApi.h>
#include <AI/BT/BTUtils.h>
#include <AI/BT/UpdateConfig.h>

namespace GLEngine::AI {

class AI_API_EXPORT I_BehavioralNode {
public:
	explicit I_BehavioralNode(const T_BTNodeID id)
		: m_ID(id)
	{
	}
	virtual ~I_BehavioralNode() = default;
	void AddChild(T_BTNodeID);

	enum class UpdateResult : std::uint8_t{
		Success,
		Failure,
		Finished
	};
	/**
	 * @brief
	 * @todo Going to cast the context to the correct type
	 */
	virtual UpdateResult Update(UpdateConfig&)				 = 0;
	/**
	 * @brief	Place to validate the values, children etc.
	 * @return	true it is valid
	 */
	[[nodiscard]] virtual bool IsValid() const = 0;
	[[nodiscard]] virtual bool AddChild(const T_BTNodeID nodeID, const unsigned int index) = 0;
	[[nodiscard]] T_BTNodeID   GetID() const { return m_ID; }
	//==================================================
	// Execution
	//==================================================
	/**
	 * @brief	Called on start of the node. Should allocate the context
	 * @return	true if node is able to allocate
	 */
	[[nodiscard]] virtual bool OnInit()							   = 0;
	virtual void			   OnFinished(UpdateConfig& updateCfg) = 0;
	/**
	 * @brief	Called on finish of the node. Should deallocate the context
	 * @return	true if everything is finished and we can return to the parent
	 */
	[[nodiscard]] virtual bool CanFinish() const = 0;
	//==================================================
	// Node metrics
	//==================================================
	/**
	 * @brief
	 * @return size of context in bytes
	 */
	[[nodiscard]] virtual unsigned int GetContextSize() const = 0;
	/**
	 * @brief
	 * @return number of child slots
	 */
	[[nodiscard]] virtual unsigned int GetNumChildSlot() const = 0;

protected:
	T_BTNodeID m_ID;
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
} // namespace GLEngine::AI
