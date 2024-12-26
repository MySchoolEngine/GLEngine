#include <RendererTestStdafx.h>

#include <Renderer/Colours.h>
#include <Renderer/Textures/Filtering/LinearFiltering.h>
#include <Renderer/Textures/Storage/TextureLinearStorage.h>
#include <Renderer/Textures/TextureView.h>

namespace GLEngine::Renderer {
class TextureViewFixture : public ::testing::Test {
protected:
	TextureViewFixture()
		: storage(3, 3, 3)
		, view(&storage)
	{}
	auto	  GetPixelCoord(const glm::vec2& uv) const { return view.GetPixelCoord(uv); }
	bool	  IsOutsideBorders(const glm::uvec2& uv) const { return view.IsOutsideBorders(uv); }
	glm::vec3 GetVec3(const glm::vec2& uv) const { return view.Get<glm::vec3, T_Bilinear>(uv); }

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
	constexpr glm::vec2 bottomLeft(0, 0);
	constexpr glm::vec2 topRight(1, 1);
	constexpr glm::vec2 outside(1.1, 1);
	const auto			sampleBottomLeft = view.Get<std::uint8_t, T_Nearest>(bottomLeft, E_TextureChannel::Red);
	const auto			sampleTopRight	 = view.Get<std::uint8_t, T_Nearest>(topRight, E_TextureChannel::Red);
	const auto			sampleOutside	 = view.Get<std::uint8_t, T_Nearest>(outside, E_TextureChannel::Red);
	EXPECT_EQ(sampleBottomLeft, 1);
	EXPECT_EQ(sampleTopRight, 1);
	EXPECT_EQ(sampleOutside, 0);
}

TEST_F(TextureViewFixture, IsOutsideBorders)
{
	EXPECT_EQ(IsOutsideBorders({0, 0}), false);
	EXPECT_EQ(IsOutsideBorders({1, 1}), false);
	EXPECT_EQ(IsOutsideBorders({2, 2}), false);
	EXPECT_EQ(IsOutsideBorders({-1, 1}), true);
	EXPECT_EQ(IsOutsideBorders({5, 0}), true);
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
	// keep in mind orientation described in C_TextureView::GetPixelCoord
	// and size of the view == (3;3)
	EXPECT_EQ(GetPixelCoord({0.5, 0.5}), glm::vec2(1, 1));
	EXPECT_EQ(GetPixelCoord({0.0, 0.0}), glm::vec2(0, 2));
	EXPECT_EQ(GetPixelCoord({1.0, 1.0}), glm::vec2(2, 0));
	EXPECT_EQ(GetPixelCoord({0.0, 1.0}), glm::vec2(0, 0));
	EXPECT_EQ(GetPixelCoord({1.0, 0.0}), glm::vec2(2, 2));
}

TEST_F(TextureViewFixture, Get_ChannelsCorrectness)
{
	view.ClearColor({Colours::white, 0.f});
	EXPECT_EQ(GetVec3({0.f, 0.f}), Colours::white);
	EXPECT_EQ(GetVec3({0.f, 1.f}), Colours::white);
	EXPECT_EQ(GetVec3({1.f, 0.f}), Colours::white);
	EXPECT_EQ(GetVec3({1.f, 1.f}), Colours::white);
}
TEST_F(TextureViewFixture, GetUVForPixel)
{
	EXPECT_PRED_FORMAT2(AssertVec2AlmostEq<float>, view.GetUVForPixel({0, 0}), glm::vec2(1.f / 6.f, 1.f - (1.f / 6.f)));
	EXPECT_PRED_FORMAT2(AssertVec2AlmostEq<float>, view.GetUVForPixel({0, 2}), glm::vec2(1.f / 6.f, 1.f / 6.f));
	EXPECT_PRED_FORMAT2(AssertVec2AlmostEq<float>, view.GetUVForPixel({2, 0}), glm::vec2(1.f - (1.f / 6.f), 1.f - (1.f / 6.f)));
	EXPECT_PRED_FORMAT2(AssertVec2AlmostEq<float>, view.GetUVForPixel({2, 2}), glm::vec2(1.f - (1.f / 6.f), 1.f / 6.f));
}
} // namespace GLEngine::Renderer