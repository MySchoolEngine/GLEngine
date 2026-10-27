#include <RendererTestStdafx.h>

#include <Renderer/Mesh/Loading/MeshResource.h>
#include <Renderer/Mesh/Scene.h>

#include <Core/Resources/LoadingQuery.h>
#include <Core/Resources/ResourceManager.h>

#include <CommonTestUtils/ResourceTestFixture.h>

#include <filesystem>

namespace GLEngine::Renderer {

namespace {
// MeshResourceFixture/triangle.obj (Tests/Renderer/TestData/, copied next to the test binary and
// used as the working directory - see Tests/readme.md and Tests/Renderer/premake5.lua's
// debugdir) is a single triangle with explicit uv/normal data. ModelLoader's
// aiProcess_CalcTangentSpace leaves aiMesh::mTangents null without UVs, and
// _getFacePosNormalTcoords indexes it unconditionally, so the fixture provides them rather than
// tripping that separate, pre-existing hazard (out of scope for F-095).
const std::filesystem::path s_TrianglePath{"MeshResourceFixture/triangle.obj"};
} // namespace

// F-095: the bbox aggregation added to Load()/Reload() (summing every mesh's transformed bbox
// into the scene-level one) is only reachable by actually calling Load() - the test fixture used
// everywhere else (MeshResourceTestFixture::SetScene) bypasses it with a prebuilt Scene.
TEST(MeshResource, Load_AggregatesEveryMeshsTransformedBboxIntoSceneBbox)
{
	auto resource = std::make_shared<MeshResource>();

	Core::LoadingQuery		 query;
	Core::Resource::LoadCtx ctx{.m_ResMng = Core::C_ResourceManager::Instance(), .m_Query = query, .m_isBlocking = true};

	ASSERT_TRUE(resource->Load(s_TrianglePath, ctx));
	// Load() itself doesn't flip the resource to Ready - that normally happens inside
	// C_ResourceManager::LoadResource after a successful Load(). GetScene() asserts IsReady(), so
	// mark it the same way StaticMeshSystemTest's MakeReadyMeshResource() does.
	std::ignore = Core::ResourceManagerBaseFixture::CreateResourceHandle(resource);

	const auto& bbox = resource->GetScene().bbox;
	ASSERT_TRUE(bbox.IsInitialized()); // a regression that skips the aggregation leaves this at the default (uninitialized) sentinel
	EXPECT_TRUE(bbox.Contains(glm::vec3(0.f, 0.f, 0.f)));
	EXPECT_TRUE(bbox.Contains(glm::vec3(4.f, 0.f, 0.f)));
	EXPECT_TRUE(bbox.Contains(glm::vec3(0.f, 2.f, 0.f)));
	EXPECT_NEAR(bbox.m_Min.x, 0.f, 0.0001f);
	EXPECT_NEAR(bbox.m_Max.x, 4.f, 0.0001f);
	EXPECT_NEAR(bbox.m_Min.y, 0.f, 0.0001f);
	EXPECT_NEAR(bbox.m_Max.y, 2.f, 0.0001f);
}

} // namespace GLEngine::Renderer
