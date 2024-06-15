#include <gtest/gtest.h>

#include <Renderer/Colours.h>
#include <Renderer/Textures/Filtering/LinearFiltering.h>
#include <Renderer/Textures/Storage/TextureLinearStorage.h>
#include <Renderer/Textures/TextureView.h>

#include <RendererTest/GlmPrinters.h>

namespace GLEngine::Renderer {
class TextureViewFixture : public ::testing::Test {
protected:
	TextureViewFixture()
		: storage(3, 3, 3)
		, view(&storage)
	{}
	auto GetPixelCoord(const glm::vec2& uv) const { return view.GetPixelCoord(uv); }
	bool IsOutsideBorders(const glm::uvec2& uv) const { return view.IsOutsideBorders(uv); }

	C_TextureViewStorageCPU<std::uint8_t> storage;
	C_TextureView						  view;
};

TEST_F(TextureViewFixture, Border)
{
	C_TextureViewStorageCPU<std::uint8_t> storage(1, 1, 3);
	storage.SetPixel(glm::vec4(Colours::white, 0.f), 0);
	C_TextureView view(&storage);
	view.SetBorderColor(glm::vec4(Colours::black, 0.f));
	view.SetWrapFunction(E_WrapFunction::ClampToBorder);
	const glm::vec2 bottomLeft(0, 0);
	const glm::vec2 topRight(1, 1);
	const glm::vec2 outside(1.1, 1);
	const auto		sampleBottomLeft = view.Get<std::uint8_t, T_Nearest>(bottomLeft, E_TextureChannel::Red);
	const auto		sampleTopRight	 = view.Get<std::uint8_t, T_Nearest>(topRight, E_TextureChannel::Red);
	const auto		sampleOutside	 = view.Get<std::uint8_t, T_Nearest>(outside, E_TextureChannel::Red);
	EXPECT_EQ(sampleBottomLeft, 1);
	EXPECT_EQ(sampleTopRight, 1);
	EXPECT_EQ(sampleOutside, 0);
}

TEST_F(TextureViewFixture, IsOutsideBorders)
{
	EXPECT_EQ(IsOutsideBorders({0, 0}),		false);
	EXPECT_EQ(IsOutsideBorders({1, 1}),		false);
	EXPECT_EQ(IsOutsideBorders({2, 2}),		false);
	EXPECT_EQ(IsOutsideBorders({-1, 1}),	true);
	EXPECT_EQ(IsOutsideBorders({5, 0}),		true);
}

TEST_F(TextureViewFixture, UseBorderColor)
{
	view.SetWrapFunction(E_WrapFunction::ClampToBorder);
	EXPECT_EQ(view.UseBorderColor(), true);
	view.SetWrapFunction(E_WrapFunction::ClampToEdge);
	EXPECT_EQ(view.UseBorderColor(), false);
	view.SetWrapFunction(E_WrapFunction::MirroredRepeat);
	EXPECT_EQ(view.UseBorderColor(), false);
	view.SetWrapFunction(E_WrapFunction::Repeat);
	EXPECT_EQ(view.UseBorderColor(), false);
}

TEST_F(TextureViewFixture, GetPixelCoord)
{
	EXPECT_EQ(GetPixelCoord({0.0, 0.0}), glm::vec2(0.5, 0.5));
}
} // namespace GLEngine::Renderer