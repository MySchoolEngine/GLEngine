#include <RendererTestStdafx.h>

#include <Renderer/Mesh/Geometry.h>

#include <Utils/StdVectorUtils.h>

#include <gmock/gmock.h>

namespace GLEngine::Renderer::MeshData {

class GeometryPlane : public ::testing::TestWithParam<float> {};

INSTANTIATE_TEST_SUITE_P(PlaneSize, GeometryPlane, ::testing::Values(.5f, 1.f, 2.f, 10.f));

TEST_P(GeometryPlane, CreatePlane)
{
	const auto plane = C_Geometry::CreatePlane(GetParam(), 0);
	EXPECT_EQ(plane.vertices.size(), 6);
	EXPECT_EQ(plane.normals.size(), 6);
	EXPECT_EQ(plane.texcoords.size(), 6);
	EXPECT_TRUE(Utils::contains(plane.texcoords, glm::vec2{1.f, 1.f}));
	EXPECT_TRUE(Utils::contains(plane.texcoords, glm::vec2{0.f, 1.f}));
	EXPECT_TRUE(Utils::contains(plane.texcoords, glm::vec2{1.f, 0.f}));
	EXPECT_TRUE(Utils::contains(plane.texcoords, glm::vec2{0.f, 0.f}));

	const auto expectedCorners = GetParam() / 2.f;
	EXPECT_TRUE(Utils::contains(plane.vertices, glm::vec3(-expectedCorners, 0.f, -expectedCorners)));
	EXPECT_TRUE(Utils::contains(plane.vertices, glm::vec3(-expectedCorners, 0.f, expectedCorners)));
	EXPECT_TRUE(Utils::contains(plane.vertices, glm::vec3(expectedCorners, 0.f, -expectedCorners)));
	EXPECT_TRUE(Utils::contains(plane.vertices, glm::vec3(expectedCorners, 0.f, expectedCorners)));
	for (const auto& texCoord : plane.texcoords)
	{
		EXPECT_THAT(texCoord.x, testing::AllOf(testing::Ge(0.f), testing::Le(1.f)));
		EXPECT_THAT(texCoord.y, testing::AllOf(testing::Ge(0.f), testing::Le(1.f)));
	}
}
} // namespace GLEngine::Renderer::MeshData