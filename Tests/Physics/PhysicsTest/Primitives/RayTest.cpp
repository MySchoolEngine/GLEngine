#include <PhysicsTestStdafx.h>

#include <Physics/Primitives/Ray.h>

#include <glm/gtx/transform.hpp>

namespace GLEngine::Physics::Primitives {

// ============================================================================
// S_Ray::TransformRay
// ============================================================================
// TransformRay is meant to move a ray into another space (e.g. world -> model)
// by transforming the origin as a point and the direction as a vector (ignoring
// translation, applying rotation/scale). These tests derive the expected values
// from the same matrix operations the implementation should perform, so
// tolerances can stay tight.

class RayTransformFixture : public ::testing::Test {
protected:
	static constexpr float EPSILON = 1e-4f;

	static glm::vec3 TransformPoint(const glm::mat4& mat, const glm::vec3& point) { return glm::vec3(mat * glm::vec4(point, 1.0f)); }

	static glm::vec3 TransformVector(const glm::mat4& mat, const glm::vec3& vector) { return glm::vec3(mat * glm::vec4(vector, 0.0f)); }

	static void ExpectVec3Near(const glm::vec3& actual, const glm::vec3& expected, float epsilon = EPSILON)
	{
		EXPECT_NEAR(actual.x, expected.x, epsilon);
		EXPECT_NEAR(actual.y, expected.y, epsilon);
		EXPECT_NEAR(actual.z, expected.z, epsilon);
	}
};

TEST_F(RayTransformFixture, IdentityTransformLeavesRayUnchanged)
{
	S_Ray ray(glm::vec3(1, 2, 3), glm::vec3(0, 0, -1));

	ray.TransformRay(glm::mat4(1.0f));

	EXPECT_PRED_FORMAT2((AssertVecAlmostEq<3, float>), ray.origin, glm::vec3(1, 2, 3));
	EXPECT_PRED_FORMAT2((AssertVecAlmostEq<3, float>), ray.direction, glm::vec3(0, 0, -1));
}

TEST_F(RayTransformFixture, TranslationMovesOriginButNotDirection)
{
	const glm::mat4 mat = glm::translate(glm::mat4(1.0f), glm::vec3(5, 5, 5));
	S_Ray			ray(glm::vec3(1, 2, 3), glm::vec3(0, 0, -1));

	ray.TransformRay(mat);

	EXPECT_PRED_FORMAT2((AssertVecAlmostEq<3, float>), ray.origin, TransformPoint(mat, glm::vec3(1, 2, 3)));
	// A translation must not affect a direction vector.
	EXPECT_PRED_FORMAT2((AssertVecAlmostEq<3, float>), ray.direction, glm::vec3(0, 0, -1));
}

TEST_F(RayTransformFixture, RotationTransformsDirectionAsAVectorNotAPoint)
{
	const glm::mat4 mat = glm::rotate(glm::radians(90.0f), glm::vec3(0, 0, 1));
	S_Ray			ray(glm::vec3(1, 0, 0), glm::vec3(1, 0, 0));

	ray.TransformRay(mat);

	EXPECT_PRED_FORMAT2((AssertVecAlmostEq<3, float>), ray.origin, TransformPoint(mat, glm::vec3(1, 0, 0)));
	EXPECT_PRED_FORMAT2((AssertVecAlmostEq<3, float>), ray.direction, TransformVector(mat, glm::vec3(1, 0, 0)));
}

TEST_F(RayTransformFixture, RotationPreservesDirectionMagnitude)
{
	const glm::mat4 mat = glm::rotate(glm::radians(37.0f), glm::normalize(glm::vec3(1, 1, 1)));
	S_Ray			ray(glm::vec3(0, 0, 0), glm::vec3(0, 0, -1));

	ray.TransformRay(mat);

	EXPECT_NEAR(glm::length(ray.direction), 1.0f, EPSILON);
}

TEST_F(RayTransformFixture, UniformScaleTransformsOriginAndDirection)
{
	const glm::mat4 mat = glm::scale(glm::mat4(1.0f), glm::vec3(2, 2, 2));
	S_Ray			ray(glm::vec3(1, 1, 1), glm::vec3(1, 0, 0));

	ray.TransformRay(mat);

	EXPECT_PRED_FORMAT2((AssertVecAlmostEq<3, float>), ray.origin, TransformPoint(mat, glm::vec3(1, 1, 1)));
	EXPECT_PRED_FORMAT2((AssertVecAlmostEq<3, float>), ray.direction, TransformVector(mat, glm::vec3(1, 0, 0)));
}

TEST_F(RayTransformFixture, NonUniformScaleAppliesPerAxisToDirection)
{
	const glm::mat4 mat = glm::scale(glm::mat4(1.0f), glm::vec3(1, 3, 1));
	S_Ray			ray(glm::vec3(1, 1, 1), glm::vec3(0, 1, 0));

	ray.TransformRay(mat);

	EXPECT_PRED_FORMAT2((AssertVecAlmostEq<3, float>), ray.origin, TransformPoint(mat, glm::vec3(1, 1, 1)));
	EXPECT_PRED_FORMAT2((AssertVecAlmostEq<3, float>), ray.direction, TransformVector(mat, glm::vec3(0, 1, 0)));
}

TEST_F(RayTransformFixture, CombinedTranslationRotationScaleTransformsBothFields)
{
	const glm::mat4 mat
		= glm::translate(glm::mat4(1.0f), glm::vec3(5, 0, 0)) * glm::rotate(glm::radians(90.0f), glm::vec3(0, 0, 1)) * glm::scale(glm::mat4(1.0f), glm::vec3(2, 2, 2));
	S_Ray ray(glm::vec3(1, 0, 0), glm::vec3(1, 0, 0));

	ray.TransformRay(mat);

	EXPECT_PRED_FORMAT2((AssertVecAlmostEq<3, float>), ray.origin, TransformPoint(mat, glm::vec3(1, 0, 0)));
	EXPECT_PRED_FORMAT2((AssertVecAlmostEq<3, float>), ray.direction, TransformVector(mat, glm::vec3(1, 0, 0)));
}

TEST_F(RayTransformFixture, ScaledDirectionMakesLocalTEqualWorldRayLength)
{
	// C_Trimesh::Intersect transforms the incoming ray into local space via TransformRay and then
	// reports the local intersection's `t` directly as the world ray length (SetRayLength(t)), with
	// no rescaling step. That is only correct if TransformRay scales direction by the matrix's linear
	// part instead of renormalizing it: linear(worldTransform) * linear(invWorldTransform) cancels out,
	// so origin + t * direction reconstructs the exact world hit point/distance once transformed back.
	const glm::mat4 worldTransform = glm::scale(glm::mat4(1.0f), glm::vec3(2, 2, 2));
	const glm::mat4 invTransform   = glm::inverse(worldTransform);

	const glm::vec3 worldOrigin	   = glm::vec3(0.6f, 0.6f, 1.0f);
	const glm::vec3 worldDirection = glm::vec3(0, 0, -1); // unit vector, as rays are normally given

	S_Ray localRay(worldOrigin, worldDirection);
	localRay.TransformRay(invTransform);

	// Local triangle sits at (0,0,0)-(1,0,0)-(0,1,0); after the 2x world scale it covers
	// (0,0,0)-(2,0,0)-(0,2,0), so the ray at world (0.6,0.6) should hit at world distance 1.0.
	ASSERT_NE(localRay.direction.z, 0.0f);
	const float		localT	 = -localRay.origin.z / localRay.direction.z; // solve for local z == 0
	const glm::vec3 localHit = localRay.origin + localT * localRay.direction;

	EXPECT_NEAR(localT, 1.0f, EPSILON) << "local t must already equal the world ray length, unscaled";
	EXPECT_PRED_FORMAT2((AssertVecAlmostEq<3, float>), localHit, glm::vec3(0.3f, 0.3f, 0.0f));
}

TEST_F(RayTransformFixture, TransformThenTransformByInverseRecoversOriginalRay)
{
	const glm::mat4 mat
		= glm::translate(glm::mat4(1.0f), glm::vec3(3, -2, 4)) * glm::rotate(glm::radians(52.0f), glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 0.5f, 2.0f));
	const glm::vec3 originalOrigin	  = glm::vec3(0.7f, -1.2f, 3.3f);
	const glm::vec3 originalDirection = glm::normalize(glm::vec3(0.2f, 0.9f, -0.4f));

	S_Ray ray(originalOrigin, originalDirection);
	ray.TransformRay(mat);
	ray.TransformRay(glm::inverse(mat));

	// Chained forward+inverse transforms accumulate more floating point noise
	// than a single application, so this uses an absolute-epsilon comparison
	// rather than the ULP-tight one above.
	ExpectVec3Near(ray.origin, originalOrigin);
	ExpectVec3Near(ray.direction, originalDirection);
}

} // namespace GLEngine::Physics::Primitives
