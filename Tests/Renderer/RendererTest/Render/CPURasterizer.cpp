#include <RendererTestStdafx.h>

#include <Renderer/Render/CPURasterizer.h>
#include <Renderer/Textures/Storage/TextureLinearStorage.h>
#include <Renderer/Textures/TextureView.h>

namespace GLEngine::Renderer {
/**
 * Fixture with default 3x3 RGB storage
 */
class CPURasterizerFixture : public ::testing::Test {
protected:
	CPURasterizerFixture()
		: storage(100, 100, 3)
		, view(&storage)
		, rasterizer(view)
	{
	}

	void SetUp() override { view.ClearColor({Colours::white, 1.f}); }

	C_TextureViewStorageCPU<std::uint8_t> storage;
	C_TextureView						  view;
	C_CPURasterizer						  rasterizer;
};

TEST_F(CPURasterizerFixture, DrawAABox)
{
	rasterizer.DrawAABox(Colours::red, {10, 10, 10, 10});
	EXPECT_EQ(view.Get<glm::vec3>(glm::uvec2(0, 0)), Colours::white);
	EXPECT_EQ(view.Get<glm::vec3>(glm::uvec2(10, 10)), Colours::red);
	EXPECT_EQ(view.Get<glm::vec3>(glm::uvec2(19, 19)), Colours::red);


	EXPECT_EQ(view.Get<glm::vec3>(glm::uvec2(20, 10)), Colours::white) << "Pixels just outside the rect";
	EXPECT_EQ(view.Get<glm::vec3>(glm::uvec2(20, 20)), Colours::white) << "Pixels just outside the rect";
	EXPECT_EQ(view.Get<glm::vec3>(glm::uvec2(19, 20)), Colours::white) << "Pixels just outside the rect";
	EXPECT_EQ(view.Get<glm::vec3>(glm::uvec2(20, 19)), Colours::white) << "Pixels just outside the rect";
}

TEST_F(CPURasterizerFixture, DrawAABox_PartiallyOutside)
{
	rasterizer.DrawAABox(Colours::red, {95, 95, 10, 10});
	EXPECT_EQ(view.Get<glm::vec3>(glm::uvec2(0, 0)), Colours::white);
	EXPECT_EQ(view.Get<glm::vec3>(glm::uvec2(95, 95)), Colours::red);
	EXPECT_EQ(view.Get<glm::vec3>(glm::uvec2(99, 99)), Colours::red);
}

} // namespace GLEngine::Renderer