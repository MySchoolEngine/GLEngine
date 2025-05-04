#include <AITestStdafx.h>

#include <AI/BT/BehavioralNode.h>
#include <AI/BT/BehavioralTree.h>
#include <AI/BT/BehaviourTreeExecutor.h>
#include <AI/BT/Nodes/BranchNodeBT.h>
#include <gmock/gmock.h>


namespace GLEngine::AI {
class C_BehavioralTreeExecutorFixture : public ::testing::Test {
protected:
	C_BehavioralTreeExecutorFixture()
		: m_Executor(&m_BehavioralTree)
	{ }

	~C_BehavioralTreeExecutorFixture() override = default;

	C_BehavioralTree m_BehavioralTree;
	C_BehaviourTreeExecutor m_Executor;
};

TEST_F(C_BehavioralTreeExecutorFixture, CreateContext)
{
	const auto nodeID = m_BehavioralTree.AddNode<C_BranchNodeBT>();
	const auto nodeInstanceID = m_Executor.StartNewInstance<C_BranchNodeBT::T_Context>(nodeID);
	{
		auto* Context = m_Executor.GetContext(nodeInstanceID);
		EXPECT_NE(Context, nullptr);
		static_cast<C_BranchNodeBT::T_Context*>(Context)->activeChild = s_InvalidNodeInstanceID;
	}
	{
		auto* Context = m_Executor.GetContext(nodeInstanceID);
		EXPECT_NE(Context, nullptr);
		EXPECT_EQ(static_cast<C_BranchNodeBT::T_Context*>(Context)->activeChild, s_InvalidNodeInstanceID);
	}
	}
} // namespace GLEngine::AI
