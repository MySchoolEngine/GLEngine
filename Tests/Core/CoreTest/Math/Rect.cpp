#include <Core/Math/Rect.h>

#include <Physics/2D/Plane2D.h>

#include <GlmPrinters.h>

#include <gtest/gtest.h>


namespace GLEngine::Core {

// test
static_assert(S_Rect(0, 0, 1, 1).IsValid());
static_assert(S_Rect(0, 0, 5, 5).Contains(glm::ivec2{1, 1}));
static_assert(!S_Rect(0, 0, 5, 5).Contains(glm::ivec2{5, 5}));
static_assert(!S_Rect(0, 0, 5, 5).GetIntersection(S_Rect(5, 5, 1, 1)).IsValid());
// try corners
static_assert(S_Rect(0, 0, 5, 5).GetIntersection(S_Rect(0, 0, 1, 1)).IsValid());
static_assert(S_Rect(0, 0, 5, 5).GetIntersection(S_Rect(0, 4, 1, 1)).IsValid());
static_assert(S_Rect(0, 0, 5, 5).GetIntersection(S_Rect(4, 0, 1, 1)).IsValid());
static_assert(S_Rect(0, 0, 5, 5).GetIntersection(S_Rect(4, 4, 1, 1)).IsValid());

TEST(Rect, Edges)
{
	S_Rect rect(1, 1, 5, 5);

	EXPECT_EQ(rect.Bottom(), 5);
	EXPECT_EQ(rect.Right(), 5);
	EXPECT_EQ(rect.Top(), 1);
	EXPECT_EQ(rect.Left(), 1);
}
TEST(Rect, Corners)
{
	S_Rect rect(1, 1, 5, 50);

	EXPECT_EQ(rect.TopLeft(), glm::uvec2(1, 1));
	EXPECT_EQ(rect.TopRight(), glm::uvec2(5, 1));
	EXPECT_EQ(rect.BottomLeft(), glm::uvec2(1, 50));
	EXPECT_EQ(rect.BottomRight(), glm::uvec2(5, 50));
}

TEST(Rect, IntersectionPlane)
{
	S_Rect				 rect(0, 0, 800, 600);
	std::array planes = {
		Physics::Primitives::Plane2D{.Normal = glm::normalize(glm::vec2{0.5f, 0.5f}), .Position = {200, 200}},
		Physics::Primitives::Plane2D{.Normal = glm::normalize(glm::vec2{-0.5f, 0.5f}), .Position = {600, 200}},
		Physics::Primitives::Plane2D{.Normal = glm::normalize(glm::vec2{0.5f, -0.5f}), .Position = {200, 400}},
		Physics::Primitives::Plane2D{.Normal = glm::normalize(glm::vec2{-0.5f, -0.5f}), .Position = {600, 400}},
	};
	std::array expectedResults = {
		std::pair{glm::ivec2{400, 0}, glm::ivec2{0, 400}},
		std::pair{glm::ivec2{400, 0}, glm::ivec2{799, 400}},
		std::pair{glm::ivec2{0, 200}, glm::ivec2{400, 599}},
		std::pair{glm::ivec2{799, 200}, glm::ivec2{400, 599}},
	};
	int i = 0;
	for (const auto& plane : planes)
	{
		const auto direction = plane.GetDirection();
		std::array<glm::ivec2, 2> Intersections;
		EXPECT_TRUE(rect.IntersectionPlane(plane.Position, direction, Intersections));
		EXPECT_TRUE(rect.Contains(Intersections[0])) << std::string("Intersection [") + std::to_string(Intersections[0].x) + ";" + std::to_string(Intersections[0].y) + "] is outside the original rect.";
		EXPECT_TRUE(rect.Contains(Intersections[1])) << std::string("Intersection [") + std::to_string(Intersections[1].x) + ";" + std::to_string(Intersections[1].y) + "] is outside the original rect.";
		// EXPECT_EQ(Intersections[0].x, expectedResults[i].first.x);
		// EXPECT_EQ(Intersections[0].y, expectedResults[i].first.y);
		// EXPECT_EQ(Intersections[1].x, expectedResults[i].second.x);
		// EXPECT_EQ(Intersections[1].y, expectedResults[i].second.y);
		++i;
	}
}
} // namespace GLEngine::Core