#include <RendererTestStdafx.h>

#include <Renderer/RayCasting/Geometry/RayTraceScene.h>
#include <Renderer/RayCasting/Geometry/StaticRTMesh.h>
#include <Renderer/RayCasting/Geometry/Trimesh.h>
#include <Renderer/RayCasting/Material/DiffuseMaterial.h>
#include <Renderer/RayCasting/RayIntersection.h>
#include <Renderer/Textures/Storage/TextureLinearStorage.h>
#include <Renderer/Textures/TextureResource.h>

#include <Physics/Primitives/Ray.h>
#include <Physics/Primitives/Triangle.h>

#include <CommonTestUtils/ResourceTestFixture.h>

#include <glm/gtx/transform.hpp>

#include <RendererTest/RayTracing/Geometry/RTGeometryTestFixture.h>

namespace GLEngine::Renderer {

// ============================================================================
// C_RayTraceScene — alpha-masked surfaces within a single trimesh
// ============================================================================
// Regression coverage for two bugs found while investigating why alpha-cutout foliage made whole
// meshes disappear in the ray tracer:
//  1) C_StaticRTMesh::Intersect compared submeshes by iteration order instead of distance (fixed by
//     guarding the "best hit" overwrite with a distance check).
//  2) C_RayTraceScene::Intersect's alpha-retry loop offsets the ray and re-intersects, so a hit found
//     after retrying is only relative to the *offset* ray, not the original one passed in by the
//     caller — these tests check that the final reported C_RayIntersection::GetRayLength() is the
//     true end-to-end distance, not the (shorter) distance from the post-offset origin.

class RayTraceSceneAlphaFixture : public RTGeometryTestFixture {
protected:
	// A 1x1 fully-transparent texture. Sampling any UV on a 1x1 texture always lands on its single
	// texel, so the front triangle doesn't need real texture coordinates for this to work as an
	// alpha mask reporting alpha == 0 everywhere.
	static Core::ResourceHandle<TextureResource> MakeFullyTransparentTexture()
	{
		auto storage = std::make_unique<C_TextureViewStorageCPU<float>>(1, 1, 4);
		storage->SetPixel(glm::vec4(1.f, 1.f, 1.f, 0.f), 0);

		auto texture			  = std::make_shared<TextureResource>();
		texture->m_TextureStorage = std::move(storage);
		return Core::ResourceManagerBaseFixture::CreateResourceHandle<TextureResource>(texture);
	}

	// Two triangles sharing the same (x,y) footprint (covering local point (0.25, 0.25)), one at
	// z=frontZ ("front") and one at z=backZ ("back"), as two submeshes of a single
	// C_TrimeshModel/C_StaticRTMesh — i.e. exactly the "alpha-masked surface with another surface of
	// the same trimesh behind it" scenario. Defaults match the original untransformed tests below.
	//
	// frontHasAlphaMask selects whether the front submesh is given the fully-transparent alpha mask
	// (so the ray should pass through it) or no alpha mask at all (so it should block the ray, as a
	// control case proving the front triangle is actually being hit).
	[[nodiscard]] std::shared_ptr<C_StaticRTMesh> MakeFrontBackMesh(bool frontHasAlphaMask, float frontZ = 5.f, float backZ = 0.f) const
	{
		std::vector<C_Trimesh> trimeshes;
		trimeshes.push_back(MakeTriangleMesh({0, 0, frontZ}, {1, 0, frontZ}, {0, 1, frontZ}));
		trimeshes.push_back(MakeTriangleMesh({0, 0, backZ}, {1, 0, backZ}, {0, 1, backZ}));

		auto mesh = std::make_shared<C_StaticRTMesh>(MakeModel(std::move(trimeshes)));
		// One (material, alphaMap) entry per trimesh — see StaticRTMesh's InitMaterials(); populated
		// directly here since these tests don't need the real MaterialResource/PBR pipeline.
		mesh->m_Materials.push_back(m_Material.get());
		mesh->m_AlphaMaps.emplace_back(frontHasAlphaMask ? MakeFullyTransparentTexture() : Core::ResourceHandle<TextureResource>{});
		mesh->m_Materials.push_back(m_Material.get());
		mesh->m_AlphaMaps.emplace_back(); // back submesh is always opaque
		return mesh;
	}

	// A single opaque triangle (no alpha mask, no retry involved) at the given z, covering the same
	// (0.25, 0.25) local point as MakeFrontBackMesh's triangles — a separate scene object standing in
	// for e.g. an unrelated wall or prop the alpha-masked mesh happens to be layered in front of/behind.
	[[nodiscard]] std::shared_ptr<C_StaticRTMesh> MakeSingleOpaqueTriangleMesh(float z) const
	{
		std::vector<C_Trimesh> trimeshes;
		trimeshes.push_back(MakeTriangleMesh({0, 0, z}, {1, 0, z}, {0, 1, z}));

		auto mesh = std::make_shared<C_StaticRTMesh>(MakeModel(std::move(trimeshes)));
		mesh->m_Materials.push_back(m_Material.get());
		mesh->m_AlphaMaps.emplace_back();
		return mesh;
	}

	// A single semi-transparent triangle at the given z, with no other geometry in this object — used
	// to test the ray passing through it *entirely* (nothing left to hit within this same object) and
	// on to hit a separate opaque object further along, as opposed to MakeFrontBackMesh's front/back
	// pair living in the *same* trimesh.
	[[nodiscard]] std::shared_ptr<C_StaticRTMesh> MakeSingleAlphaMaskedTriangleMesh(float z) const
	{
		std::vector<C_Trimesh> trimeshes;
		trimeshes.push_back(MakeTriangleMesh({0, 0, z}, {1, 0, z}, {0, 1, z}));

		auto mesh = std::make_shared<C_StaticRTMesh>(MakeModel(std::move(trimeshes)));
		mesh->m_Materials.push_back(m_Material.get());
		mesh->m_AlphaMaps.emplace_back(MakeFullyTransparentTexture());
		return mesh;
	}
};

TEST_F(RayTraceSceneAlphaFixture, RayPassesThroughAlphaMaskedFrontSurfaceAndHitsOpaqueBackSurfaceWithCorrectLength)
{
	C_RayTraceScene scene;
	scene.AddObject(MakeFrontBackMesh(/*frontHasAlphaMask=*/true));

	constexpr Physics::Primitives::S_Ray ray(glm::vec3(0.25f, 0.25f, 10.f), glm::vec3(0, 0, -1));
	C_RayIntersection				 intersection;

	ASSERT_TRUE(scene.Intersect(ray, intersection));
	// True end-to-end distance from the ray's real origin (z=10) to the back triangle (z=0), not the
	// distance from wherever the alpha-retry offset moved the internal search ray to.
	EXPECT_NEAR(intersection.GetRayLength(), 10.f, EPSILON);
	EXPECT_PRED_FORMAT2((AssertVecAlmostEq<3, float>), intersection.GetIntersectionPoint(), glm::vec3(0.25f, 0.25f, 0.f));
}

TEST_F(RayTraceSceneAlphaFixture, OpaqueFrontSurfaceBlocksRayFromReachingBackSurface)
{
	// Control case: without an alpha mask, the closer (front) triangle must win — confirms the
	// previous test's "pass-through" result comes from alpha handling, not from the front triangle
	// being missed for some other reason.
	C_RayTraceScene scene;
	scene.AddObject(MakeFrontBackMesh(/*frontHasAlphaMask=*/false));

	constexpr Physics::Primitives::S_Ray ray(glm::vec3(0.25f, 0.25f, 10.f), glm::vec3(0, 0, -1));
	C_RayIntersection				 intersection;

	ASSERT_TRUE(scene.Intersect(ray, intersection));
	EXPECT_NEAR(intersection.GetRayLength(), 5.f, EPSILON);
	EXPECT_PRED_FORMAT2((AssertVecAlmostEq<3, float>), intersection.GetIntersectionPoint(), glm::vec3(0.25f, 0.25f, 5.f));
}

TEST_F(RayTraceSceneAlphaFixture, RayPassesThroughAlphaMaskedFrontSurfaceOnTransformedMeshWithCorrectLength)
{
	// Same front/back setup as the untransformed test, but the whole C_StaticRTMesh carries a
	// world transform (translation + rotation, no scale) via SetTransformation(). C_StaticRTMesh::
	// Intersect transforms the incoming ray into local space via m_InvTransform and transforms the
	// result back via m_Transform, so this exercises the same alpha-retry length bug through that
	// extra local<->world round trip, instead of only in the identity-transform case.
	auto			mesh	  = MakeFrontBackMesh(/*frontHasAlphaMask=*/true);
	const glm::mat4 transform = glm::translate(glm::mat4(1.f), glm::vec3(5, -3, 7)) * glm::rotate(glm::radians(40.f), glm::vec3(0, 1, 0));
	mesh->SetTransformation(transform);

	C_RayTraceScene scene;
	scene.AddObject(std::move(mesh));

	// Ray built by transforming the same local ray used in the untransformed test, so it hits the
	// same local points (0.25,0.25,10) -> (0.25,0.25,0) regardless of the mesh's world placement.
	const glm::vec3					 worldOrigin = glm::vec3(transform * glm::vec4(0.25f, 0.25f, 10.f, 1.f));
	const glm::vec3					 worldDir	 = glm::normalize(glm::vec3(transform * glm::vec4(0, 0, -1, 0.f)));
	const Physics::Primitives::S_Ray ray(worldOrigin, worldDir);
	C_RayIntersection				 intersection;

	ASSERT_TRUE(scene.Intersect(ray, intersection));
	// transform is rigid (no scale), so local and world distances match: still 10, end-to-end from
	// the ray's true origin to the back triangle.
	EXPECT_NEAR(intersection.GetRayLength(), 10.f, EPSILON);

	const glm::vec3 expectedWorldPoint = glm::vec3(transform * glm::vec4(0.25f, 0.25f, 0.f, 1.f));
	// Rotation runs the expected point through sin/cos, so it can pick up last-bit floating point
	// noise relative to the ULP-tight AssertVecAlmostEq comparison used in the untransformed tests
	// above (same reasoning as StaticRTMeshTests.cpp's rotation cases).
	EXPECT_PRED_FORMAT3((AssertVecNear<3, float>), intersection.GetIntersectionPoint(), expectedWorldPoint, EPSILON);
}

TEST_F(RayTraceSceneAlphaFixture, ClosestOverallHitWinsEvenWhenALaterObjectsAlphaRetryLandsWithinAStaleTMax)
{
	// Regression test for a third, scene-level bug: within the alpha-retry loop, `closestIntersect.t`
	// is a *global* length (from the ray's true origin — it's what ultimately wins across objects),
	// but `inter.GetRayLength()` and the tMax fed into `object->Intersect(localRay, inter,
	// closestIntersect.t)` are *local* to the current (possibly already-offset) search ray.
	// `traveledSoFar` is never subtracted before that comparison/tMax pass-down. So once one object has
	// already set a finite closestIntersect.t, a *later* object's post-retry hit can look like it's
	// within bounds locally even though, in true distance, it is farther than the already-found best —
	// and incorrectly overwrites it.
	C_RayTraceScene scene;
	// Object A (added first, so processed first): opaque, true distance 6 from the ray origin — must
	// win overall.
	scene.AddObject(MakeSingleOpaqueTriangleMesh(/*z=*/4.f));
	// Object B (added second, so closestIntersect.t == 6 from A is already set when B is processed):
	// transparent front at true distance 1, opaque back at true distance 6.5 — farther than A, so it
	// must lose. Its back hit is only reachable via a retry, which is exactly the path that loses track
	// of how far the ray has already traveled.
	scene.AddObject(MakeFrontBackMesh(/*frontHasAlphaMask=*/true, /*frontZ=*/9.f, /*backZ=*/3.5f));

	constexpr Physics::Primitives::S_Ray ray(glm::vec3(0.25f, 0.25f, 10.f), glm::vec3(0, 0, -1));
	C_RayIntersection				 intersection;

	ASSERT_TRUE(scene.Intersect(ray, intersection));
	EXPECT_NEAR(intersection.GetRayLength(), 6.f, EPSILON);
	EXPECT_PRED_FORMAT2((AssertVecAlmostEq<3, float>), intersection.GetIntersectionPoint(), glm::vec3(0.25f, 0.25f, 4.f));
}

// ============================================================================
// C_RayTraceScene — semi-transparent object with a *separate* opaque object behind it
// ============================================================================
// Unlike MakeFrontBackMesh (front/back submeshes of the *same* trimesh), here the semi-transparent
// surface and the opaque surface behind it are two entirely separate scene objects (separate
// C_RayTraceScene::AddObject calls, each with its own C_StaticRTMesh/BVH). The alpha-retry loop only
// re-intersects the *same* object, so passing through the transparent object should make it
// contribute nothing at all to closestIntersect, leaving the opaque object's independently-cast hit
// as the answer — regardless of which of the two was added to the scene first.

TEST_F(RayTraceSceneAlphaFixture, RayPassesThroughStandaloneTransparentObjectAndHitsSeparateOpaqueObjectBehindIt)
{
	C_RayTraceScene scene;
	scene.AddObject(MakeSingleAlphaMaskedTriangleMesh(/*z=*/7.f)); // transparent, true distance 3 -- added first
	scene.AddObject(MakeSingleOpaqueTriangleMesh(/*z=*/2.f));	   // opaque, true distance 8

	constexpr Physics::Primitives::S_Ray ray(glm::vec3(0.25f, 0.25f, 10.f), glm::vec3(0, 0, -1));
	C_RayIntersection				 intersection;

	ASSERT_TRUE(scene.Intersect(ray, intersection));
	EXPECT_NEAR(intersection.GetRayLength(), 8.f, EPSILON);
	EXPECT_PRED_FORMAT2((AssertVecAlmostEq<3, float>), intersection.GetIntersectionPoint(), glm::vec3(0.25f, 0.25f, 2.f));
}

TEST_F(RayTraceSceneAlphaFixture, RayPassesThroughStandaloneTransparentObjectAndHitsSeparateOpaqueObjectAddedBeforeIt)
{
	// Same scenario as above with the two objects added in the opposite order, so the opaque hit's
	// distance (8) is already the finite closestIntersect.t by the time the transparent object's retry
	// runs — exercising the same ordering sensitivity as ClosestOverallHitWinsEvenWhen... above, but
	// where the transparent object genuinely has nothing behind it and must correctly contribute nothing.
	C_RayTraceScene scene;
	scene.AddObject(MakeSingleOpaqueTriangleMesh(/*z=*/2.f));	   // opaque, true distance 8 -- added first this time
	scene.AddObject(MakeSingleAlphaMaskedTriangleMesh(/*z=*/7.f)); // transparent, true distance 3

	constexpr Physics::Primitives::S_Ray ray(glm::vec3(0.25f, 0.25f, 10.f), glm::vec3(0, 0, -1));
	C_RayIntersection				 intersection;

	ASSERT_TRUE(scene.Intersect(ray, intersection));
	EXPECT_NEAR(intersection.GetRayLength(), 8.f, EPSILON);
	EXPECT_PRED_FORMAT2((AssertVecAlmostEq<3, float>), intersection.GetIntersectionPoint(), glm::vec3(0.25f, 0.25f, 2.f));
}

} // namespace GLEngine::Renderer
