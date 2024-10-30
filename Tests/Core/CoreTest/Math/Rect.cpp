#include <Core/Math/Rect.h>

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
	S_Rect rect(1, 1, 5, 5);

	// EXPECT_EQ(rect.TopLeft(), glm::uvec2{1, 1});
	// EXPECT_EQ(rect.TopRight(), glm::uvec2{1, 5});
	// EXPECT_EQ(rect.BottomLeft(), glm::uvec2{5, 1});
	// EXPECT_EQ(rect.BottomRight(), glm::uvec2{5, 5});
}
} // namespace GLEngine::Core