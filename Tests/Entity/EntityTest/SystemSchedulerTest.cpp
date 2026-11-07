#include <EntityTestStdafx.h>

#include <Entity/Systems/SystemScheduler.h>
#include <Entity/World.h>

namespace GLEngine::Entity {

TEST(SystemScheduler, RunsStagesInOrder_PreUpdateThenUpdateThenPostUpdate)
{
	C_World					 world;
	SystemScheduler			 scheduler;
	std::vector<std::string> order;

	// The registration order is different from the run order intentionally
	scheduler.AddSystem(E_SchedulerStage::PostUpdate, [&order](C_World&) { order.emplace_back("Post"); });
	scheduler.AddSystem(E_SchedulerStage::Update, [&order](C_World&) { order.emplace_back("Update"); });
	scheduler.AddSystem(E_SchedulerStage::PreUpdate, [&order](C_World&) { order.emplace_back("Pre"); });

	scheduler.Run(world);

	ASSERT_EQ(order.size(), 3u);
	EXPECT_EQ(order[0], "Pre");
	EXPECT_EQ(order[1], "Update");
	EXPECT_EQ(order[2], "Post");
}

TEST(SystemScheduler, MultipleSystemsInSameStage_RunInRegistrationOrder)
{
	C_World			 world;
	SystemScheduler	 scheduler;
	std::vector<int> order;

	scheduler.AddSystem(E_SchedulerStage::Update, [&order](C_World&) { order.push_back(1); });
	scheduler.AddSystem(E_SchedulerStage::Update, [&order](C_World&) { order.push_back(2); });

	scheduler.Run(world);

	ASSERT_EQ(order.size(), 2u);
	EXPECT_EQ(order[0], 1);
	EXPECT_EQ(order[1], 2);
}

} // namespace GLEngine::Entity
