#include <AITestStdafx.h>

#include <AI/BT/BehavioralNode.h>
#include <AI/BT/BehavioralTree.h>
#include <AI/BT/Nodes/ActionNodeBT.h>
#include <AI/BT/Nodes/BranchNodeBT.h>
#include <gmock/gmock.h>


namespace GLEngine::AI {
class C_BehavioralTreeFixture : public ::testing::Test {
protected:
	C_BehavioralTreeFixture() = default;

	~C_BehavioralTreeFixture() override = default;

	static T_BTNodeID GetPositiveChild(const C_BranchNodeBT* branchNode) { return branchNode->m_Child1; }
	static T_BTNodeID GetNegativeChild(const C_BranchNodeBT* branchNode) { return branchNode->m_Child2; }

	C_BehavioralTree m_BehavioralTree;
};

TEST_F(C_BehavioralTreeFixture, AddNode)
{
	EXPECT_EQ(m_BehavioralTree.GetNumNodes(), 0);
	const auto id = m_BehavioralTree.AddNode<C_BranchNodeBT>();
	EXPECT_EQ(m_BehavioralTree.GetNumNodes(), 1);
	const auto node = m_BehavioralTree.GetNode(id);
	EXPECT_EQ(id, node->GetID());
}

TEST_F(C_BehavioralTreeFixture, AddChild)
{
	EXPECT_EQ(m_BehavioralTree.GetNumNodes(), 0);
	const auto branchID = m_BehavioralTree.AddNode<C_BranchNodeBT>();
	EXPECT_EQ(m_BehavioralTree.GetNumNodes(), 1);
	const auto* node = m_BehavioralTree.GetNode(branchID);
	const auto* branchNode = dynamic_cast<const C_BranchNodeBT*>(node);
	EXPECT_EQ(branchID, node->GetID());

	const auto positiveID = m_BehavioralTree.AddNode<C_ActionNodeBT>();
	const auto negativeID = m_BehavioralTree.AddNode<C_ActionNodeBT>();

	EXPECT_FALSE(m_BehavioralTree.AddChild(positiveID, negativeID, 0));
	EXPECT_FALSE(m_BehavioralTree.AddChild(positiveID, negativeID, 2));
	EXPECT_TRUE(m_BehavioralTree.AddChild(branchID, positiveID, C_BranchNodeBT::Branches::Positive));
	EXPECT_TRUE(m_BehavioralTree.AddChild(branchID, negativeID, C_BranchNodeBT::Branches::Negative));
	EXPECT_EQ(GetPositiveChild(branchNode), positiveID);
	EXPECT_EQ(GetNegativeChild(branchNode), negativeID);
}
} // namespace GLEngine::AI
