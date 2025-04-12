#pragma once

#include <Core/Filesystem/Paths.h>
#include <Core/Resources/Resource.h>
#include <Core/Resources/ResourceLoader.h>

#include <AI/AIApi.h>
#include <AI/BT/BTUtils.h>
#include <string>

namespace GLEngine::Core {
class I_ResourceLoader;
}
namespace GLEngine::AI {
class I_BehavioralNode;

class AI_API_EXPORT C_BehavioralTree : public Core::Resource {
public:
	DEFINE_RESOURCE_WITH_PATH_TYPE(C_BehavioralTree, Core::Filesystem::AIFolder)
	//==================================================
	// Editing the graph
	//==================================================
	// Adds a node to the tree and returns its ID
	template <class NodeType> requires std::derived_from<NodeType, I_BehavioralNode> T_BTNodeID AddNode()
	{
		// Temporary solution
		const T_BTNodeID ret  = static_cast<T_BTNodeID>(m_Nodes.size());
		auto			 node = new NodeType(ret);
		m_Nodes.push_back(node);
		return ret;
	}

	/**
	 * @brief Adds a child to the parent node. If the parent node is not a branch, it will generate error.
	 * @param parent
	 * @param child
	 * @return true if successful
	 */
	bool			  AddChild(T_BTNodeID parent, T_BTNodeID child);
	I_BehavioralNode* GetNode(T_BTNodeID nodeID) { return nullptr; }
	bool			  LoadFromResource(const std::string& resourcePath); // New function declaration

	// Core::Resource overrides
	std::unique_ptr<Core::I_ResourceLoader> GetLoader() override;
	bool									Load(const std::filesystem::path& filepath) override { return false; }
	bool									Reload() override { return false; }

	std::size_t GetNumNodes() const { return m_Nodes.size(); }

private:
	std::vector<I_BehavioralNode*> m_Nodes;
	RTTR_REGISTRATION_FRIEND;
};


class AI_API_EXPORT BehavioralTreeLoader : public Core::ResourceLoader<C_BehavioralTree> {
public:
	std::shared_ptr<Core::Resource> CreateResource() const override { return std::make_shared<C_BehavioralTree>(); }
	std::vector<std::string>		GetSupportedExtensions() const override { return {".bt"}; }
};

} // namespace GLEngine::AI