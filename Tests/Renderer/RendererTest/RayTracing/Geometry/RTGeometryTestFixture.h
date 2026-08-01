#pragma once

#include <gtest/gtest.h>

#include <Renderer/RayCasting/Geometry/Trimesh.h>
#include <Renderer/RayCasting/Geometry/TrimeshModel.h>
#include <Renderer/RayCasting/Material/DiffuseMaterial.h>

#include <Physics/Primitives/Triangle.h>

#include <CommonTestUtils/ResourceTestFixture.h>

#include <glm/glm.hpp>

namespace GLEngine::Renderer {

// Shared setup for tests that build C_Trimesh/C_TrimeshModel geometry by hand: a stand-in
// C_DiffuseMaterial (these tests don't need the real MaterialResource/PBR pipeline) plus helpers
// for turning raw triangles into a resource-handle-backed C_TrimeshModel.
class RTGeometryTestFixture : public ::testing::Test {
protected:
	static constexpr float EPSILON = 1e-4f;

	void SetUp() override { m_Material = std::make_unique<C_DiffuseMaterial>(glm::vec3(1.0f, 1.0f, 1.0f)); }

	[[nodiscard]] static C_Trimesh MakeTriangleMesh(const glm::vec3& v0 = glm::vec3(0, 0, 0), const glm::vec3& v1 = glm::vec3(1, 0, 0), const glm::vec3& v2 = glm::vec3(0, 1, 0))
	{
		C_Trimesh  trimesh;
		const auto tri = Physics::Primitives::S_Triangle::Create(v0, v1, v2);
		EXPECT_TRUE(tri.has_value());
		trimesh.AddTriangle(tri.value());
		return trimesh;
	}

	[[nodiscard]] static Core::ResourceHandle<C_TrimeshModel> MakeModel(std::vector<C_Trimesh> trimeshes)
	{
		auto model		   = std::make_shared<C_TrimeshModel>();
		model->m_Trimeshes = std::move(trimeshes);
		return Core::ResourceManagerBaseFixture::CreateResourceHandle<C_TrimeshModel>(model);
	}

	std::unique_ptr<C_DiffuseMaterial> m_Material;
};

} // namespace GLEngine::Renderer
