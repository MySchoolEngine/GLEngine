#include <RendererTestStdafx.h>

#include <Renderer/RayCasting/Generator/Sampler.h>
#include <Renderer/RayCasting/Geometry/GeometryTraits.h>
#include <Renderer/RayCasting/RayIntersection.h>

#include <Physics/Primitives/Rectangle.h>

namespace GLEngine::Renderer::RayTracing {

// ============================================================================
// T_GeometryTraits overloads for S_Rectangle
// ============================================================================

class GeometryTraitsRectangleFixture : public ::testing::Test {
protected:
	static constexpr float EPSILON = 1e-4f;

	// Rectangle centered at the origin, facing +Z, right = +X, up = +Y.
	static Physics::Primitives::S_Rectangle MakeAxisAlignedRect(float halfWidth = 2.f, float halfHeight = 1.5f)
	{
		return Physics::Primitives::S_Rectangle(Physics::Primitives::S_Plane(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0)), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), halfWidth,
												halfHeight);
	}
};

TEST_F(GeometryTraitsRectangleFixture, GetAreaMatchesWidthTimesHeight)
{
	const auto rect = MakeAxisAlignedRect(2.f, 1.5f);

	EXPECT_NEAR(T_GeometryTraits::GetArea(rect), 12.f, EPSILON); // (2*2) * (2*1.5)
}

TEST_F(GeometryTraitsRectangleFixture, GetNormalReturnsPlaneNormal)
{
	const auto rect = MakeAxisAlignedRect();

	EXPECT_PRED_FORMAT2((AssertVecAlmostEq<3, float>), T_GeometryTraits::GetNormal(rect), glm::vec3(0, 0, 1));
}

TEST_F(GeometryTraitsRectangleFixture, SamplePointStaysWithinExtents)
{
	const auto	 rect = MakeAxisAlignedRect(2.f, 1.5f);
	C_STDSampler sampler(0.f, 1.f);

	for (int i = 0; i < 200; ++i)
	{
		const auto sample = T_GeometryTraits::SamplePoint(rect, sampler);
		const auto offset = sample - rect.plane.origin;
		const auto x	  = glm::dot(offset, rect.right);
		const auto y	  = glm::dot(offset, rect.up);

		EXPECT_LE(std::abs(x), rect.halfWidth + EPSILON);
		EXPECT_LE(std::abs(y), rect.halfHeight + EPSILON);
	}
}

TEST_F(GeometryTraitsRectangleFixture, FillIntersectionProducesForwardFacingNormal)
{
	const auto						 rect = MakeAxisAlignedRect();
	const Physics::Primitives::S_Ray ray(glm::vec3(0, 0, 5), glm::vec3(0, 0, -1));
	const auto						 t = rect.IntersectImpl(ray, std::numeric_limits<float>::infinity());
	ASSERT_GE(t, 0.f);

	C_RayIntersection intersection;
	T_GeometryTraits::FillIntersection(rect, t, ray, intersection);

	// The ray approaches from +Z along -Z, so the frame normal must face back toward the ray (+Z).
	EXPECT_PRED_FORMAT2((AssertVecAlmostEq<3, float>), intersection.GetFrame().Normal(), glm::vec3(0, 0, 1));
}

} // namespace GLEngine::Renderer::RayTracing
