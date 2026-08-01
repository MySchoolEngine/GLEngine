#include <RendererTestStdafx.h>

#include <Renderer/RayCasting/Geometry/StaticRTMesh.h>
#include <Renderer/RayCasting/Geometry/Trimesh.h>
#include <Renderer/RayCasting/Geometry/TrimeshModel.h>
#include <Renderer/RayCasting/Material/DiffuseMaterial.h>
#include <Renderer/RayCasting/RayIntersection.h>

#include <Physics/Primitives/Triangle.h>

#include <glm/gtx/transform.hpp>

#include <RendererTest/RayTracing/Geometry/RTGeometryTestFixture.h>

namespace GLEngine::Renderer {

// ============================================================================
// Test Fixture
// ============================================================================

class StaticRTMeshFixture : public RTGeometryTestFixture {
protected:
	static glm::vec3 TransformPoint(const glm::mat4& transform, const glm::vec3& point) { return glm::vec3(transform * glm::vec4(point, 1.0f)); }

	static glm::vec3 TransformDirection(const glm::mat4& transform, const glm::vec3& direction) { return glm::vec3(transform * glm::vec4(direction, 0.0f)); }

	// Builds a ray that hits `localPoint` (given in the mesh's local/object space) once `transform`
	// is applied, approaching from `distance` away along the forward-transformed local normal. Using
	// the same transform to derive both the ray and the expected world-space point (rather than
	// hand-computed literals) keeps the two numerically consistent under rotation and scale.
	static Physics::Primitives::S_Ray MakeRayHitting(const glm::mat4& transform, const glm::vec3& localPoint, const glm::vec3& localNormal, float distance = 1.0f)
	{
		const glm::vec3 worldPoint	= TransformPoint(transform, localPoint);
		const glm::vec3 worldNormal = glm::normalize(TransformDirection(transform, localNormal));
		return Physics::Primitives::S_Ray(worldPoint + worldNormal * distance, -worldNormal);
	}

	// Normals need the inverse-transpose of the linear part to stay correct under non-uniform scale.
	static glm::vec3 TransformNormal(const glm::mat4& transform, const glm::vec3& normal)
	{
		return glm::normalize(glm::vec3(glm::transpose(glm::inverse(transform)) * glm::vec4(normal, 0.0f)));
	}

	// Single triangle occupying (0,0,0), (1,0,0), (0,1,0) in local/object space unless overridden.
	[[nodiscard]] C_StaticRTMesh
	MakeSingleTriangleMesh(const glm::vec3& v0 = glm::vec3(0, 0, 0), const glm::vec3& v1 = glm::vec3(1, 0, 0), const glm::vec3& v2 = glm::vec3(0, 1, 0)) const
	{
		std::vector<C_Trimesh> meshes;
		meshes.push_back(MakeTriangleMesh(v0, v1, v2));
		C_StaticRTMesh mesh(MakeModel(std::move(meshes)));
		mesh.SetMaterial(m_Material.get());
		// C_StaticRTMesh::Intersect iterates zip(trimeshes, m_Materials, m_AlphaMaps), so these need one
		// entry per trimesh — normally populated by InitMaterials() from real MaterialResource handles,
		// which these geometry/transform-focused tests don't need. StaticRTMeshFixture is a friend
		// specifically so this can be filled in directly instead.
		mesh.m_Materials.push_back(m_Material.get());
		mesh.m_AlphaMaps.emplace_back();
		return mesh;
	}

	// Same as MakeSingleTriangleMesh, but the single trimesh has a BVH attached, so
	// C_StaticRTMesh::Intersect exercises C_Trimesh::Intersect's BVH-accelerated branch instead of
	// IntersectBruteforce. This helper (not the TEST_F body) needs to populate m_Materials/m_AlphaMaps
	// directly, since friendship isn't inherited: TEST_F bodies run in a gtest-generated subclass of
	// StaticRTMeshFixture, which is not itself a friend of C_StaticRTMesh.
	[[nodiscard]] C_StaticRTMesh
	MakeSingleTriangleMeshWithBVH(const glm::vec3& v0 = glm::vec3(0, 0, 0), const glm::vec3& v1 = glm::vec3(1, 0, 0), const glm::vec3& v2 = glm::vec3(0, 1, 0)) const
	{
		C_Trimesh trimesh = MakeTriangleMesh(v0, v1, v2);
		auto*	  bvh	  = new BVH(trimesh);
		bvh->Build();
		trimesh.SetBVH(bvh); // trimesh now owns bvh

		std::vector<C_Trimesh> meshes;
		meshes.push_back(std::move(trimesh));
		C_StaticRTMesh mesh(MakeModel(std::move(meshes)));
		mesh.SetMaterial(m_Material.get());
		mesh.m_Materials.push_back(m_Material.get());
		mesh.m_AlphaMaps.emplace_back();
		return mesh;
	}

	// Builds a model made of two parts (e.g. a tree's trunk + crown), each carrying its own local
	// placement transform baked in via C_Trimesh::SetTransformation(). This mirrors how a real
	// multipart model is authored: parts are offset relative to each other in the model's own
	// space, and the whole model is meant to move together as one unit via C_StaticRTMesh's own
	// transform on top.
	static Core::ResourceHandle<C_TrimeshModel> MakeTwoPartModelHandle(const glm::mat4& partATransform, const glm::mat4& partBTransform)
	{
		C_Trimesh partA = MakeTriangleMesh();
		partA.SetTransformation(partATransform);
		C_Trimesh partB = MakeTriangleMesh();
		partB.SetTransformation(partBTransform);

		std::vector<C_Trimesh> meshes;
		meshes.push_back(std::move(partA));
		meshes.push_back(std::move(partB));
		return MakeModel(std::move(meshes));
	}

	[[nodiscard]] C_StaticRTMesh MakeTwoPartModel(const glm::mat4& partATransform, const glm::mat4& partBTransform) const
	{
		C_StaticRTMesh mesh(MakeTwoPartModelHandle(partATransform, partBTransform));
		mesh.SetMaterial(m_Material.get());
		// One entry per trimesh (two parts) — see comment in MakeSingleTriangleMesh.
		mesh.m_Materials.push_back(m_Material.get());
		mesh.m_AlphaMaps.emplace_back();
		mesh.m_Materials.push_back(m_Material.get());
		mesh.m_AlphaMaps.emplace_back();
		return mesh;
	}

	static constexpr glm::vec3 s_LocalNormal = glm::vec3(0, 0, 1); // normal of the default triangle, (0,0,0)-(1,0,0)-(0,1,0)
};

// ============================================================================
// Identity Transform (no SetTransformation call / no-op transform)
// ============================================================================

TEST_F(StaticRTMeshFixture, IdentityTransformHitsUntransformedTriangle)
{
	C_StaticRTMesh mesh = MakeSingleTriangleMesh();

	constexpr glm::mat4 identity   = glm::mat4(1.0f);
	constexpr glm::vec3 localPoint = glm::vec3(0.25f, 0.25f, 0.0f);
	const auto			ray		   = MakeRayHitting(identity, localPoint, s_LocalNormal);
	C_RayIntersection	hit;

	ASSERT_TRUE(mesh.Intersect(ray, hit, std::numeric_limits<float>::infinity()));
	EXPECT_NEAR(hit.GetRayLength(), 1.0f, EPSILON);
	EXPECT_PRED_FORMAT2((AssertVecAlmostEq<3, float>), hit.GetIntersectionPoint(), TransformPoint(identity, localPoint));
}

TEST_F(StaticRTMeshFixture, IdentityTransformMissesWhenRayDoesNotHitTriangle)
{
	C_StaticRTMesh mesh = MakeSingleTriangleMesh();

	constexpr Physics::Primitives::S_Ray ray{glm::vec3(10.0f, 10.0f, 1.0f), glm::vec3(0, 0, -1)};
	C_RayIntersection					 hit;

	EXPECT_FALSE(mesh.Intersect(ray, hit, std::numeric_limits<float>::infinity()));
}

// ============================================================================
// Translation
// ============================================================================

TEST_F(StaticRTMeshFixture, TranslationAlongXMovesHitPosition)
{
	C_StaticRTMesh mesh = MakeSingleTriangleMesh();

	const glm::mat4		transform  = glm::translate(glm::mat4(1.0f), glm::vec3(5, 0, 0));
	constexpr glm::vec3 localPoint = glm::vec3(0.25f, 0.25f, 0.0f);
	mesh.SetTransformation(transform);

	const auto		  ray = MakeRayHitting(transform, localPoint, s_LocalNormal);
	C_RayIntersection hit;

	ASSERT_TRUE(mesh.Intersect(ray, hit, std::numeric_limits<float>::infinity()));
	EXPECT_NEAR(hit.GetRayLength(), 1.0f, EPSILON);
	EXPECT_PRED_FORMAT2((AssertVecAlmostEq<3, float>), hit.GetIntersectionPoint(), TransformPoint(transform, localPoint));
	EXPECT_PRED_FORMAT2((AssertVecAlmostEq<3, float>), hit.GetFrame().Normal(), TransformNormal(transform, s_LocalNormal));
}

TEST_F(StaticRTMeshFixture, TranslationAlongYMovesHitPosition)
{
	C_StaticRTMesh mesh = MakeSingleTriangleMesh();

	const glm::mat4		transform  = glm::translate(glm::mat4(1.0f), glm::vec3(0, 5, 0));
	constexpr glm::vec3 localPoint = glm::vec3(0.25f, 0.25f, 0.0f);
	mesh.SetTransformation(transform);

	const auto		  ray = MakeRayHitting(transform, localPoint, s_LocalNormal);
	C_RayIntersection hit;

	ASSERT_TRUE(mesh.Intersect(ray, hit, std::numeric_limits<float>::infinity()));
	EXPECT_NEAR(hit.GetRayLength(), 1.0f, EPSILON);
	EXPECT_PRED_FORMAT2((AssertVecAlmostEq<3, float>), hit.GetIntersectionPoint(), TransformPoint(transform, localPoint));
}

TEST_F(StaticRTMeshFixture, TranslationAlongZChangesRayLength)
{
	C_StaticRTMesh mesh = MakeSingleTriangleMesh();

	const glm::mat4		transform  = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 5));
	constexpr glm::vec3 localPoint = glm::vec3(0.25f, 0.25f, 0.0f);
	mesh.SetTransformation(transform);

	const auto		  ray = MakeRayHitting(transform, localPoint, s_LocalNormal, /*distance*/ 3.0f);
	C_RayIntersection hit;

	ASSERT_TRUE(mesh.Intersect(ray, hit, std::numeric_limits<float>::infinity()));
	EXPECT_NEAR(hit.GetRayLength(), 3.0f, EPSILON);
	EXPECT_PRED_FORMAT2((AssertVecAlmostEq<3, float>), hit.GetIntersectionPoint(), TransformPoint(transform, localPoint));
}

TEST_F(StaticRTMeshFixture, TranslationCausesOriginalSpaceRayToMiss)
{
	C_StaticRTMesh mesh = MakeSingleTriangleMesh();
	mesh.SetTransformation(glm::translate(glm::mat4(1.0f), glm::vec3(5, 5, 5)));

	// Ray that used to hit the untransformed triangle should now miss, since the geometry moved away.
	constexpr Physics::Primitives::S_Ray ray{glm::vec3(0.25f, 0.25f, 1.0f), glm::vec3(0, 0, -1)};
	C_RayIntersection					 hit;

	EXPECT_FALSE(mesh.Intersect(ray, hit, std::numeric_limits<float>::infinity()));
}

TEST_F(StaticRTMeshFixture, CombinedXYZTranslation)
{
	C_StaticRTMesh mesh = MakeSingleTriangleMesh();

	const glm::mat4		transform  = glm::translate(glm::mat4(1.0f), glm::vec3(2, 3, 4));
	constexpr glm::vec3 localPoint = glm::vec3(0.25f, 0.25f, 0.0f);
	mesh.SetTransformation(transform);

	const auto		  ray = MakeRayHitting(transform, localPoint, s_LocalNormal);
	C_RayIntersection hit;

	ASSERT_TRUE(mesh.Intersect(ray, hit, std::numeric_limits<float>::infinity()));
	EXPECT_NEAR(hit.GetRayLength(), 1.0f, EPSILON);
	EXPECT_PRED_FORMAT2((AssertVecAlmostEq<3, float>), hit.GetIntersectionPoint(), TransformPoint(transform, localPoint));
}

// ============================================================================
// Rotation
// ============================================================================

TEST_F(StaticRTMeshFixture, Rotation90DegreesAroundZMovesHitPositionInPlane)
{
	C_StaticRTMesh mesh = MakeSingleTriangleMesh();

	const glm::mat4		transform  = glm::rotate(glm::radians(90.0f), glm::vec3(0, 0, 1));
	constexpr glm::vec3 localPoint = glm::vec3(0.25f, 0.25f, 0.0f);
	mesh.SetTransformation(transform);

	const auto		  ray = MakeRayHitting(transform, localPoint, s_LocalNormal);
	C_RayIntersection hit;

	ASSERT_TRUE(mesh.Intersect(ray, hit, std::numeric_limits<float>::infinity()));
	EXPECT_NEAR(hit.GetRayLength(), 1.0f, EPSILON);
	EXPECT_PRED_FORMAT2((AssertVecAlmostEq<3, float>), hit.GetIntersectionPoint(), TransformPoint(transform, localPoint));
	// Rotation about Z does not tilt a Z-facing normal.
	EXPECT_PRED_FORMAT2((AssertVecAlmostEq<3, float>), hit.GetFrame().Normal(), TransformNormal(transform, s_LocalNormal));
}

TEST_F(StaticRTMeshFixture, Rotation90DegreesAroundZCausesOriginalSpaceRayToMiss)
{
	C_StaticRTMesh mesh = MakeSingleTriangleMesh();
	mesh.SetTransformation(glm::rotate(glm::radians(90.0f), glm::vec3(0, 0, 1)));

	Physics::Primitives::S_Ray ray{glm::vec3(0.25f, 0.25f, 1.0f), glm::vec3(0, 0, -1)};
	C_RayIntersection		   hit;

	EXPECT_FALSE(mesh.Intersect(ray, hit, std::numeric_limits<float>::infinity()));
}

TEST_F(StaticRTMeshFixture, Rotation90DegreesAroundXReorientsTriangleIntoXZPlane)
{
	C_StaticRTMesh mesh = MakeSingleTriangleMesh();

	const glm::mat4		transform  = glm::rotate(glm::radians(90.0f), glm::vec3(1, 0, 0));
	constexpr glm::vec3 localPoint = glm::vec3(0.25f, 0.25f, 0.0f);
	mesh.SetTransformation(transform);

	const auto		  ray = MakeRayHitting(transform, localPoint, s_LocalNormal);
	C_RayIntersection hit;

	ASSERT_TRUE(mesh.Intersect(ray, hit, std::numeric_limits<float>::infinity()));
	EXPECT_NEAR(hit.GetRayLength(), 1.0f, EPSILON);
	EXPECT_PRED_FORMAT2((AssertVecAlmostEq<3, float>), hit.GetIntersectionPoint(), TransformPoint(transform, localPoint));
	// Local normal (0,0,1) rotates to (0,-1,0).
	EXPECT_PRED_FORMAT2((AssertVecAlmostEq<3, float>), hit.GetFrame().Normal(), TransformNormal(transform, s_LocalNormal));

	// A ray along the original (pre-rotation) facing direction is now parallel to the triangle and must miss.
	Physics::Primitives::S_Ray originalRay{glm::vec3(0.25f, 0.25f, 1.0f), glm::vec3(0, 0, -1)};
	C_RayIntersection		   originalHit;
	EXPECT_FALSE(mesh.Intersect(originalRay, originalHit, std::numeric_limits<float>::infinity()));
}

TEST_F(StaticRTMeshFixture, Rotation90DegreesAroundYReorientsTriangleIntoYZPlane)
{
	C_StaticRTMesh mesh = MakeSingleTriangleMesh();

	const glm::mat4		transform  = glm::rotate(glm::radians(90.0f), glm::vec3(0, 1, 0));
	constexpr glm::vec3 localPoint = glm::vec3(0.25f, 0.25f, 0.0f);
	mesh.SetTransformation(transform);

	const auto		  ray = MakeRayHitting(transform, localPoint, s_LocalNormal);
	C_RayIntersection hit;

	ASSERT_TRUE(mesh.Intersect(ray, hit, std::numeric_limits<float>::infinity()));
	EXPECT_NEAR(hit.GetRayLength(), 1.0f, EPSILON);
	EXPECT_PRED_FORMAT2((AssertVecAlmostEq<3, float>), hit.GetIntersectionPoint(), TransformPoint(transform, localPoint));
	// Local normal (0,0,1) rotates to (1,0,0).
	EXPECT_PRED_FORMAT2((AssertVecAlmostEq<3, float>), hit.GetFrame().Normal(), TransformNormal(transform, s_LocalNormal));
}

TEST_F(StaticRTMeshFixture, RotationAboutAxisPreservesDistanceForPointOnThatAxis)
{
	C_StaticRTMesh mesh = MakeSingleTriangleMesh();
	// Vertex (0,0,0) lies exactly on the Z rotation axis, so an arbitrary angle must not move it.
	mesh.SetTransformation(glm::rotate(glm::radians(37.0f), glm::vec3(0, 0, 1)));

	constexpr Physics::Primitives::S_Ray ray{glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0, 0, -1)};
	C_RayIntersection					 hit;

	ASSERT_TRUE(mesh.Intersect(ray, hit, std::numeric_limits<float>::infinity()));
	EXPECT_NEAR(hit.GetRayLength(), 1.0f, EPSILON);
	// AssertVecAlmostEq's ULP-based comparison is too strict here: rotating by an arbitrary angle
	// still runs the point through sin/cos, so even a point exactly on the rotation axis can pick up
	// last-bit floating-point noise rather than landing on a bit-exact zero.
	EXPECT_PRED_FORMAT3((AssertVecNear<3, float>), hit.GetIntersectionPoint(), glm::vec3(0.0f, 0.0f, 0.0f), EPSILON);
}

TEST_F(StaticRTMeshFixture, FullCircleRotationBehavesLikeIdentity)
{
	C_StaticRTMesh mesh = MakeSingleTriangleMesh();
	mesh.SetTransformation(glm::rotate(glm::radians(360.0f), glm::vec3(0, 1, 0)));

	constexpr Physics::Primitives::S_Ray ray{glm::vec3(0.25f, 0.25f, 1.0f), glm::vec3(0, 0, -1)};
	C_RayIntersection					 hit;

	ASSERT_TRUE(mesh.Intersect(ray, hit, std::numeric_limits<float>::infinity()));
	EXPECT_NEAR(hit.GetRayLength(), 1.0f, EPSILON);
	// A 360-degree rotation is not a bit-exact identity matrix in floating point (sin/cos of
	// radians(360) aren't exactly 0/1), so use EPSILON tolerance rather than AssertVecAlmostEq's
	// ULP-based comparison.
	EXPECT_PRED_FORMAT3((AssertVecNear<3, float>), hit.GetIntersectionPoint(), glm::vec3(0.25f, 0.25f, 0.0f), EPSILON);
}

// ============================================================================
// Scale
// ============================================================================

TEST_F(StaticRTMeshFixture, UniformScaleProducesCorrectHitPositionAndLength)
{
	C_StaticRTMesh mesh = MakeSingleTriangleMesh();

	const glm::mat4 transform = glm::scale(glm::mat4(1.0f), glm::vec3(2, 2, 2));
	mesh.SetTransformation(transform);

	constexpr glm::vec3 localPoint = glm::vec3(0.3f, 0.3f, 0.0f);
	const auto			ray		   = MakeRayHitting(transform, localPoint, s_LocalNormal);
	C_RayIntersection	hit;

	ASSERT_TRUE(mesh.Intersect(ray, hit, std::numeric_limits<float>::infinity()));
	EXPECT_NEAR(hit.GetRayLength(), 1.0f, EPSILON);
	EXPECT_PRED_FORMAT2((AssertVecAlmostEq<3, float>), hit.GetIntersectionPoint(), TransformPoint(transform, localPoint));
	// Uniform scale only needs renormalizing, not redirecting.
	EXPECT_PRED_FORMAT2((AssertVecAlmostEq<3, float>), hit.GetFrame().Normal(), TransformNormal(transform, s_LocalNormal));
}

TEST_F(StaticRTMeshFixture, RayHitsOriginalTriangleButMissesAfterUniformScaleDown)
{
	C_StaticRTMesh mesh = MakeSingleTriangleMesh();
	mesh.SetTransformation(glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f)));

	// (0.3, 0.3) is inside the local unit triangle, but outside the half-scale one (x + y <= 0.5).
	constexpr Physics::Primitives::S_Ray ray{glm::vec3(0.3f, 0.3f, 1.0f), glm::vec3(0, 0, -1)};
	C_RayIntersection					 hit;

	EXPECT_FALSE(mesh.Intersect(ray, hit, std::numeric_limits<float>::infinity()));
}

TEST_F(StaticRTMeshFixture, UniformScaleChangesRayLengthAlongZ)
{
	C_StaticRTMesh mesh = MakeSingleTriangleMesh(glm::vec3(0, 0, 1), glm::vec3(1, 0, 1), glm::vec3(0, 1, 1));

	const glm::mat4		transform  = glm::scale(glm::mat4(1.0f), glm::vec3(2, 2, 2));
	constexpr glm::vec3 localPoint = glm::vec3(0.3f, 0.3f, 1.0f);
	mesh.SetTransformation(transform);

	const auto		  ray = MakeRayHitting(transform, localPoint, s_LocalNormal);
	C_RayIntersection hit;

	ASSERT_TRUE(mesh.Intersect(ray, hit, std::numeric_limits<float>::infinity()));
	EXPECT_NEAR(hit.GetRayLength(), 1.0f, EPSILON);
	EXPECT_PRED_FORMAT2((AssertVecAlmostEq<3, float>), hit.GetIntersectionPoint(), TransformPoint(transform, localPoint));
}

TEST_F(StaticRTMeshFixture, NonUniformScaleStretchesOnlyTheScaledAxis_Y)
{
	C_StaticRTMesh mesh = MakeSingleTriangleMesh();

	const glm::mat4 transform = glm::scale(glm::mat4(1.0f), glm::vec3(1, 3, 1));
	mesh.SetTransformation(transform);

	constexpr glm::vec3 localPoint = glm::vec3(0.2f, 0.3f, 0.0f);
	const auto			ray		   = MakeRayHitting(transform, localPoint, s_LocalNormal);
	C_RayIntersection	hit;

	ASSERT_TRUE(mesh.Intersect(ray, hit, std::numeric_limits<float>::infinity()));
	EXPECT_NEAR(hit.GetRayLength(), 1.0f, EPSILON);
	EXPECT_PRED_FORMAT2((AssertVecAlmostEq<3, float>), hit.GetIntersectionPoint(), TransformPoint(transform, localPoint));
	// Non-uniform scale needs the inverse-transpose, not a direct re-apply of scale, to keep the
	// normal correct.
	EXPECT_PRED_FORMAT2((AssertVecAlmostEq<3, float>), hit.GetFrame().Normal(), TransformNormal(transform, s_LocalNormal));
}

TEST_F(StaticRTMeshFixture, NonUniformScaleStretchesOnlyTheScaledAxis_X)
{
	C_StaticRTMesh mesh = MakeSingleTriangleMesh();

	const glm::mat4 transform = glm::scale(glm::mat4(1.0f), glm::vec3(3, 1, 1));
	mesh.SetTransformation(transform);

	constexpr glm::vec3 localPoint = glm::vec3(0.3f, 0.2f, 0.0f);
	const auto			ray		   = MakeRayHitting(transform, localPoint, s_LocalNormal);
	C_RayIntersection	hit;

	ASSERT_TRUE(mesh.Intersect(ray, hit, std::numeric_limits<float>::infinity()));
	EXPECT_NEAR(hit.GetRayLength(), 1.0f, EPSILON);
	EXPECT_PRED_FORMAT2((AssertVecAlmostEq<3, float>), hit.GetIntersectionPoint(), TransformPoint(transform, localPoint));
}

TEST_F(StaticRTMeshFixture, TMaxCullsHitBeyondMaxDistanceUnderScale)
{
	C_StaticRTMesh mesh = MakeSingleTriangleMesh(glm::vec3(0, 0, 1), glm::vec3(1, 0, 1), glm::vec3(0, 1, 1));
	mesh.SetTransformation(glm::scale(glm::mat4(1.0f), glm::vec3(2, 2, 2)));

	// World hit is at distance 2 (triangle at local z=1, scaled to z=2).
	constexpr Physics::Primitives::S_Ray ray{glm::vec3(0.3f, 0.3f, 0.0f), glm::vec3(0, 0, 1)};
	C_RayIntersection					 hit;

	EXPECT_FALSE(mesh.Intersect(ray, hit, 1.5f)) << "Hit is farther than tMax and should be culled";
	EXPECT_TRUE(mesh.Intersect(ray, hit, 3.0f)) << "Hit is within tMax and should be found";
}

// C_Trimesh::Intersect's BVH branch recomputes ray length via glm::distance(origin, point) *after*
// TransformRayAndPoint(m_Transform), instead of trusting the parametric `t` BVH::IntersectNode already
// set (see TrimeshTests.cpp's BVHAcceleratedIntersectionWithUniformScaleProducesCorrectRayLength for
// the direct, one-level case, which is NOT buggy: there, the trimesh's own m_Transform is the same
// scale passed to SetTransformation, so TransformRayAndPoint correctly undoes it before the Euclidean
// recompute runs).
//
// Here the trimesh is wrapped in a C_StaticRTMesh instead. C_StaticRTMesh::Intersect applies its OWN
// outer scale by transforming the ray into static-mesh-local space *before* ever calling
// trimesh.Intersect() -- so the ray's direction already carries the 1/scale factor when it reaches
// C_Trimesh::Intersect. The wrapped C_Trimesh's own m_Transform is identity (a single-part model),
// so TransformRayAndPoint(identity) is a no-op: the Euclidean recompute stays stuck measuring
// distance in the scaled-direction frame and is never corrected back. C_StaticRTMesh::Intersect then
// treats that wrong value as if it were already-correct world distance (per the same "local t equals
// world distance" invariant IntersectBruteforce relies on), so it comes out wrong by exactly the scale
// factor once C_StaticRTMesh applies its own (correct) point/ray transform on top.
TEST_F(StaticRTMeshFixture, BVHAcceleratedUniformScaleThroughStaticRTMeshProducesCorrectRayLength)
{
	C_StaticRTMesh mesh = MakeSingleTriangleMeshWithBVH();

	const glm::mat4		transform  = glm::scale(glm::mat4(1.0f), glm::vec3(2, 2, 2));
	constexpr glm::vec3 localPoint = glm::vec3(0.3f, 0.3f, 0.0f);
	mesh.SetTransformation(transform);

	const auto		  ray = MakeRayHitting(transform, localPoint, s_LocalNormal);
	C_RayIntersection hit;

	ASSERT_TRUE(mesh.Intersect(ray, hit, std::numeric_limits<float>::infinity()));
	// MakeRayHitting places the ray exactly 1.0 world-space unit away from the hit point regardless of
	// the mesh's own scale, so the true ray length must always come out as 1.0 here.
	EXPECT_NEAR(hit.GetRayLength(), 1.0f, EPSILON);
	EXPECT_PRED_FORMAT2((AssertVecAlmostEq<3, float>), hit.GetIntersectionPoint(), TransformPoint(transform, localPoint));
}

// ============================================================================
// Combined Translation + Rotation + Scale
// ============================================================================

TEST_F(StaticRTMeshFixture, CombinedTranslationAndScale)
{
	C_StaticRTMesh mesh = MakeSingleTriangleMesh();

	const glm::mat4		transform  = glm::translate(glm::mat4(1.0f), glm::vec3(5, 5, 5)) * glm::scale(glm::mat4(1.0f), glm::vec3(2, 2, 2));
	constexpr glm::vec3 localPoint = glm::vec3(0.25f, 0.25f, 0.0f);
	mesh.SetTransformation(transform);

	const auto		  ray = MakeRayHitting(transform, localPoint, s_LocalNormal);
	C_RayIntersection hit;

	ASSERT_TRUE(mesh.Intersect(ray, hit, std::numeric_limits<float>::infinity()));
	EXPECT_NEAR(hit.GetRayLength(), 1.0f, EPSILON);
	EXPECT_PRED_FORMAT2((AssertVecAlmostEq<3, float>), hit.GetIntersectionPoint(), TransformPoint(transform, localPoint));
}

TEST_F(StaticRTMeshFixture, CombinedTranslationAndRotation)
{
	C_StaticRTMesh mesh = MakeSingleTriangleMesh();

	const glm::mat4		transform  = glm::translate(glm::mat4(1.0f), glm::vec3(5, 0, 0)) * glm::rotate(glm::radians(90.0f), glm::vec3(0, 0, 1));
	constexpr glm::vec3 localPoint = glm::vec3(0.25f, 0.25f, 0.0f);
	mesh.SetTransformation(transform);

	const auto		  ray = MakeRayHitting(transform, localPoint, s_LocalNormal);
	C_RayIntersection hit;

	ASSERT_TRUE(mesh.Intersect(ray, hit, std::numeric_limits<float>::infinity()));
	EXPECT_NEAR(hit.GetRayLength(), 1.0f, EPSILON);
	EXPECT_PRED_FORMAT2((AssertVecAlmostEq<3, float>), hit.GetIntersectionPoint(), TransformPoint(transform, localPoint));
}

TEST_F(StaticRTMeshFixture, CombinedTranslationRotationAndScale)
{
	C_StaticRTMesh mesh = MakeSingleTriangleMesh();

	const glm::mat4 transform
		= glm::translate(glm::mat4(1.0f), glm::vec3(2, 0, 0)) * glm::rotate(glm::radians(90.0f), glm::vec3(0, 0, 1)) * glm::scale(glm::mat4(1.0f), glm::vec3(2, 2, 2));
	constexpr glm::vec3 localPoint = glm::vec3(0.25f, 0.25f, 0.0f);
	mesh.SetTransformation(transform);

	const auto		  ray = MakeRayHitting(transform, localPoint, s_LocalNormal);
	C_RayIntersection hit;

	ASSERT_TRUE(mesh.Intersect(ray, hit, std::numeric_limits<float>::infinity()));
	EXPECT_NEAR(hit.GetRayLength(), 1.0f, EPSILON);
	EXPECT_PRED_FORMAT2((AssertVecAlmostEq<3, float>), hit.GetIntersectionPoint(), TransformPoint(transform, localPoint));
	EXPECT_PRED_FORMAT2((AssertVecAlmostEq<3, float>), hit.GetFrame().Normal(), TransformNormal(transform, s_LocalNormal));
}

// ============================================================================
// Re-setting the transformation
// ============================================================================

TEST_F(StaticRTMeshFixture, SettingTransformationTwiceUsesTheLatestOne)
{
	C_StaticRTMesh mesh = MakeSingleTriangleMesh();
	mesh.SetTransformation(glm::translate(glm::mat4(1.0f), glm::vec3(5, 5, 5)));

	// Original-space ray misses while the mesh is translated away.
	{
		Physics::Primitives::S_Ray ray{glm::vec3(0.25f, 0.25f, 1.0f), glm::vec3(0, 0, -1)};
		C_RayIntersection		   hit;
		EXPECT_FALSE(mesh.Intersect(ray, hit, std::numeric_limits<float>::infinity()));
	}

	// Resetting back to identity must recompute the inverse transform, not reuse a stale one.
	mesh.SetTransformation(glm::mat4(1.0f));
	{
		Physics::Primitives::S_Ray ray{glm::vec3(0.25f, 0.25f, 1.0f), glm::vec3(0, 0, -1)};
		C_RayIntersection		   hit;
		ASSERT_TRUE(mesh.Intersect(ray, hit, std::numeric_limits<float>::infinity()));
		EXPECT_NEAR(hit.GetRayLength(), 1.0f, EPSILON);
		EXPECT_PRED_FORMAT2((AssertVecAlmostEq<3, float>), hit.GetIntersectionPoint(), glm::vec3(0.25f, 0.25f, 0.0f));
	}
}

// ============================================================================
// Multi-part models (e.g. a tree's trunk + crown, each offset within the model's own space)
//
// C_TrimeshModel can hold several C_Trimesh parts, each with its own local placement transform
// baked in independently (how a part is positioned relative to the rest of the model). Moving the
// whole model is meant to happen once, via C_StaticRTMesh::SetTransformation(), on top of that --
// not by repositioning each part. These tests exercise that composition and record normal/point
// orientation for each part individually.
// ============================================================================

TEST_F(StaticRTMeshFixture, MultiPartModelEachPartKeepsItsOwnLocalPlacementAndNormal)
{
	// "Trunk" sits at the model's own origin; "crown" is offset+rotated within the model's own
	// space, independent of wherever the whole model ends up being placed in the world.
	constexpr glm::mat4 trunkTransform = glm::mat4(1.0f);
	const glm::mat4		crownTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0, 5, 0)) * glm::rotate(glm::radians(90.0f), glm::vec3(1, 0, 0));

	C_StaticRTMesh mesh = MakeTwoPartModel(trunkTransform, crownTransform);

	// Whole-model world placement, meant to apply once on top of both parts.
	const glm::mat4 worldTransform = glm::translate(glm::mat4(1.0f), glm::vec3(10, 0, 0));
	mesh.SetTransformation(worldTransform);

	constexpr glm::vec3 localPoint = glm::vec3(0.25f, 0.25f, 0.0f);

	// Ray aimed at the trunk part: the intended world-space target accounts for both the trunk's
	// own placement and the whole model's world transform.
	{
		const glm::mat4	  intendedWorldTransform = worldTransform * trunkTransform;
		const auto		  ray					 = MakeRayHitting(intendedWorldTransform, localPoint, s_LocalNormal);
		C_RayIntersection hit;

		ASSERT_TRUE(mesh.Intersect(ray, hit, std::numeric_limits<float>::infinity()));
		EXPECT_NEAR(hit.GetRayLength(), 1.0f, EPSILON);
		// The whole model's world transform must compose with the trunk's own local placement.
		EXPECT_PRED_FORMAT2((AssertVecAlmostEq<3, float>), hit.GetIntersectionPoint(), TransformPoint(intendedWorldTransform, localPoint));
		EXPECT_PRED_FORMAT2((AssertVecAlmostEq<3, float>), hit.GetFrame().Normal(), TransformNormal(intendedWorldTransform, s_LocalNormal));
	}

	// Ray aimed at the crown part, using its own (different) local placement.
	{
		const glm::mat4	  intendedWorldTransform = worldTransform * crownTransform;
		const auto		  ray					 = MakeRayHitting(intendedWorldTransform, localPoint, s_LocalNormal);
		C_RayIntersection hit;

		ASSERT_TRUE(mesh.Intersect(ray, hit, std::numeric_limits<float>::infinity()));
		EXPECT_NEAR(hit.GetRayLength(), 1.0f, EPSILON);
		EXPECT_PRED_FORMAT2((AssertVecAlmostEq<3, float>), hit.GetIntersectionPoint(), TransformPoint(intendedWorldTransform, localPoint));
		EXPECT_PRED_FORMAT2((AssertVecAlmostEq<3, float>), hit.GetFrame().Normal(), TransformNormal(intendedWorldTransform, s_LocalNormal));
	}
}

TEST_F(StaticRTMeshFixture, MultiPartModelIdentityWorldTransformMatchesEachPartsOwnPlacement)
{
	// With no world transform on the C_StaticRTMesh, each part's own local transform is exactly what
	// should come back.
	const glm::mat4 trunkTransform = glm::translate(glm::mat4(1.0f), glm::vec3(1, 0, 0));
	const glm::mat4 crownTransform = glm::scale(glm::mat4(1.0f), glm::vec3(1, 1, 2));

	C_StaticRTMesh mesh = MakeTwoPartModel(trunkTransform, crownTransform);

	constexpr glm::vec3 localPoint = glm::vec3(0.25f, 0.25f, 0.0f);

	{
		const auto		  ray = MakeRayHitting(trunkTransform, localPoint, s_LocalNormal);
		C_RayIntersection hit;

		ASSERT_TRUE(mesh.Intersect(ray, hit, std::numeric_limits<float>::infinity()));
		EXPECT_PRED_FORMAT2((AssertVecAlmostEq<3, float>), hit.GetIntersectionPoint(), TransformPoint(trunkTransform, localPoint));
		EXPECT_PRED_FORMAT2((AssertVecAlmostEq<3, float>), hit.GetFrame().Normal(), TransformNormal(trunkTransform, s_LocalNormal));
	}

	{
		const auto		  ray = MakeRayHitting(crownTransform, localPoint, s_LocalNormal);
		C_RayIntersection hit;

		ASSERT_TRUE(mesh.Intersect(ray, hit, std::numeric_limits<float>::infinity()));
		EXPECT_PRED_FORMAT2((AssertVecAlmostEq<3, float>), hit.GetIntersectionPoint(), TransformPoint(crownTransform, localPoint));
		EXPECT_PRED_FORMAT2((AssertVecAlmostEq<3, float>), hit.GetFrame().Normal(), TransformNormal(crownTransform, s_LocalNormal));
	}
}

} // namespace GLEngine::Renderer
