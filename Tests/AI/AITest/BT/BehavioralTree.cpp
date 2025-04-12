#include <AITestStdafx.h>

#include <AI/BT/BehavioralTree.h>
#include <AI/BT/BehavioralNode.h>
#include <gmock/gmock.h>


namespace GLEngine::AI {
class C_BehavioralTreeFixture : public ::testing::Test {
protected:
	C_BehavioralTreeFixture() = default;

	~C_BehavioralTreeFixture() override = default;


	C_BehavioralTree m_BehavioralTree;
};

TEST_F(C_BehavioralTreeFixture, AddNode)
{
	EXPECT_EQ(m_BehavioralTree.GetNumNodes(), 0);
	auto id = m_BehavioralTree.AddNode<C_BranchNodeBT>();
	EXPECT_EQ(m_BehavioralTree.GetNumNodes(), 1);
}
} // namespace GLEngine::Core
