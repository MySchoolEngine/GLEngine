#include <PhysicsTestStdafx.h>

#include <Physics/Primitives/Rectangle.h>

namespace GLEngine::Physics::Primitives {

// ============================================================================
// S_Rectangle::IntersectImpl
// ============================================================================
// The rectangle is an axis-aligned-in-plane quad: a plane hit test (from S_Plane,
// reused as-is) followed by rejecting hits whose projection onto the right/up axes
// falls outside [-halfWidth, halfWidth] x [-halfHeight, halfHeight].

class RectangleFixture : public ::testing::Test {
protected:
	static constexpr float EPSILON = 1e-4f;

	// Rectangle centered at the origin, facing +Z, right = +X, up = +Y.
	static S_Rectangle MakeAxisAlignedRect(float halfWidth = 1.f, float halfHeight = 0.5f, bool twoSided = true)
	{
		S_Rectangle rect(S_Plane(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0)), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), halfWidth, halfHeight);
		rect.plane.twoSided = twoSided;
		return rect;
	}
};

TEST_F(RectangleFixture, CenterHit)
{
	const auto	rect = MakeAxisAlignedRect();
	const S_Ray ray(glm::vec3(0, 0, 5), glm::vec3(0, 0, -1));

	const auto t = rect.IntersectImpl(ray, std::numeric_limits<float>::infinity());

	EXPECT_NEAR(t, 5.f, EPSILON);
}

TEST_F(RectangleFixture, RayParallelToPlaneMisses)
{
	const auto	rect = MakeAxisAlignedRect();
	const S_Ray ray(glm::vec3(0, 0, 5), glm::vec3(1, 0, 0));

	const auto t = rect.IntersectImpl(ray, std::numeric_limits<float>::infinity());

	EXPECT_LT(t, 0.f);
}

TEST_F(RectangleFixture, HitOutsideWidthMisses)
{
	const auto	rect = MakeAxisAlignedRect(/*halfWidth=*/1.f, /*halfHeight=*/0.5f);
	const S_Ray ray(glm::vec3(2, 0, 5), glm::vec3(0, 0, -1));

	const auto t = rect.IntersectImpl(ray, std::numeric_limits<float>::infinity());

	EXPECT_LT(t, 0.f);
}

TEST_F(RectangleFixture, HitOutsideHeightMisses)
{
	const auto	rect = MakeAxisAlignedRect(/*halfWidth=*/1.f, /*halfHeight=*/0.5f);
	const S_Ray ray(glm::vec3(0, 2, 5), glm::vec3(0, 0, -1));

	const auto t = rect.IntersectImpl(ray, std::numeric_limits<float>::infinity());

	EXPECT_LT(t, 0.f);
}

TEST_F(RectangleFixture, HitExactlyOnEdgeCounts)
{
	const auto	rect = MakeAxisAlignedRect(/*halfWidth=*/1.f, /*halfHeight=*/0.5f);
	const S_Ray ray(glm::vec3(1.f, 0, 5), glm::vec3(0, 0, -1));

	const auto t = rect.IntersectImpl(ray, std::numeric_limits<float>::infinity());

	EXPECT_NEAR(t, 5.f, EPSILON);
}

TEST_F(RectangleFixture, HitExactlyOnCornerCounts)
{
	const auto	rect = MakeAxisAlignedRect(/*halfWidth=*/1.f, /*halfHeight=*/0.5f);
	const S_Ray ray(glm::vec3(1.f, 0.5f, 5), glm::vec3(0, 0, -1));

	const auto t = rect.IntersectImpl(ray, std::numeric_limits<float>::infinity());

	EXPECT_NEAR(t, 5.f, EPSILON);
}

TEST_F(RectangleFixture, RespectsTMax)
{
	const auto	rect = MakeAxisAlignedRect();
	const S_Ray ray(glm::vec3(0, 0, 5), glm::vec3(0, 0, -1));

	// The hit is at t=5, so a tMax of 3 must reject it.
	const auto t = rect.IntersectImpl(ray, 3.f);

	EXPECT_LT(t, 0.f);
}

TEST_F(RectangleFixture, OneSidedMissesFromBehind)
{
	const auto rect = MakeAxisAlignedRect(1.f, 0.5f, /*twoSided=*/false);
	// Approaching the front face (normal-facing side): must hit.
	const S_Ray front(glm::vec3(0, 0, 5), glm::vec3(0, 0, -1));
	// Approaching from behind (same direction as the normal): must miss.
	const S_Ray behind(glm::vec3(0, 0, -5), glm::vec3(0, 0, 1));

	EXPECT_NEAR(rect.IntersectImpl(front, std::numeric_limits<float>::infinity()), 5.f, EPSILON);
	EXPECT_LT(rect.IntersectImpl(behind, std::numeric_limits<float>::infinity()), 0.f);
}

} // namespace GLEngine::Physics::Primitives
