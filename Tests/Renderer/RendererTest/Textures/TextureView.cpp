#include <RendererTestStdafx.h>

#include <Renderer/Colours.h>
#include <Renderer/Textures/Filtering/LinearFiltering.h>
#include <Renderer/Textures/Storage/TextureLinearStorage.h>
#include <Renderer/Textures/TextureView.h>

namespace GLEngine::Renderer {
template <int w, int h, int ch> class TextureViewFixture : public ::testing::Test {
protected:
	TextureViewFixture()
		: storage(w, h, ch)
		, view(&storage)
	{
	}
	auto					GetPixelCoord(const glm::vec2& uv) const { return view.GetPixelCoord(uv); }
	bool					IsOutsideBorders(const glm::uvec2& uv) const { return view.IsOutsideBorders(uv); }
	glm::vec3				GetVec3(const glm::vec2& uv) const { return view.Sample<glm::vec3, T_Bilinear>(uv); }
	template <class T> auto ClampCoordinates(const T& coord) const { return view.ClampCoordinates(coord); }

	C_TextureViewStorageCPU<std::uint8_t> storage;
	C_TextureView						  view;
};

using TextureViewWithAlphaFixture = TextureViewFixture<3, 3, 4>;
using TextureView3x3Fixture		  = TextureViewFixture<3, 3, 3>;
using TextureView2x2Fixture		  = TextureViewFixture<2, 2, 3>;

TEST_F(TextureView3x3Fixture, Border)
{
	C_TextureViewStorageCPU<std::uint8_t> storage(1, 1, 3);
	C_TextureView						  view(&storage);

	storage.SetPixel(glm::vec4(Colours::white, 0.f), 0);
	view.SetBorderColor(glm::vec4(Colours::black, 0.f));
	view.SetWrapFunction(E_WrapFunction::ClampToBorder);
	constexpr glm::vec2 bottomLeft(0, 0);
	constexpr glm::vec2 topRight(1, 1);
	constexpr glm::vec2 outside(1.1, 1);
	const auto			sampleBottomLeft = view.Sample<std::uint8_t, T_Nearest>(bottomLeft, E_TextureChannel::Red);
	const auto			sampleTopRight	 = view.Sample<std::uint8_t, T_Nearest>(topRight, E_TextureChannel::Red);
	const auto			sampleOutside	 = view.Sample<std::uint8_t, T_Nearest>(outside, E_TextureChannel::Red);
	EXPECT_EQ(sampleBottomLeft, 1);
	EXPECT_EQ(sampleTopRight, 1);
	EXPECT_EQ(sampleOutside, 0);
}

TEST_F(TextureView3x3Fixture, IsOutsideBorders)
{
	EXPECT_EQ(IsOutsideBorders({0, 0}), false);
	EXPECT_EQ(IsOutsideBorders({1, 1}), false);
	EXPECT_EQ(IsOutsideBorders({2, 2}), false);
	EXPECT_EQ(IsOutsideBorders({-1, 1}), true);
	EXPECT_EQ(IsOutsideBorders({5, 0}), true);
}

TEST_F(TextureView3x3Fixture, UseBorderColor)
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

TEST_F(TextureView3x3Fixture, GetPixelCoord)
{
	// keep in mind orientation described in C_TextureView::GetPixelCoord
	// and size of the view == (3;3)
	EXPECT_EQ(GetPixelCoord({0.5, 0.5}), glm::vec2(1, 1));
	EXPECT_EQ(GetPixelCoord({0.0, 0.0}), glm::vec2(0, 0));
	EXPECT_EQ(GetPixelCoord({1.0, 1.0}), glm::vec2(2, 2));
	EXPECT_EQ(GetPixelCoord({0.0, 1.0}), glm::vec2(0, 2));
	EXPECT_EQ(GetPixelCoord({1.0, 0.0}), glm::vec2(2, 0));
}

TEST_F(TextureView3x3Fixture, Get_ChannelsCorrectness)
{
	view.ClearColor({Colours::white, 0.f});
	EXPECT_EQ(GetVec3({0.f, 0.f}), Colours::white);
	EXPECT_EQ(GetVec3({0.f, 1.f}), Colours::white);
	EXPECT_EQ(GetVec3({1.f, 0.f}), Colours::white);
	EXPECT_EQ(GetVec3({1.f, 1.f}), Colours::white);
}
TEST_F(TextureView3x3Fixture, GetUVForPixel)
{
	EXPECT_PRED_FORMAT2(AssertVec2AlmostEq<float>, view.GetUVForPixel({0, 0}), glm::vec2(1.f / 6.f, 1.f - (1.f / 6.f)));
	EXPECT_PRED_FORMAT2(AssertVec2AlmostEq<float>, view.GetUVForPixel({0, 2}), glm::vec2(1.f / 6.f, 1.f / 6.f));
	EXPECT_PRED_FORMAT2(AssertVec2AlmostEq<float>, view.GetUVForPixel({2, 0}), glm::vec2(1.f - (1.f / 6.f), 1.f - (1.f / 6.f)));
	EXPECT_PRED_FORMAT2(AssertVec2AlmostEq<float>, view.GetUVForPixel({2, 2}), glm::vec2(1.f - (1.f / 6.f), 1.f / 6.f));
}

TEST_F(TextureViewWithAlphaFixture, EnableBlending)
{
	view.EnableBlending(true);
	C_TextureView::PixelCoordVec coord{1, 1};
	view.DrawPixel(coord, glm::vec4{Colours::white, 1.f}); // there was a bug when alpha channel haven't got propagated
	EXPECT_EQ(view.Get<glm::vec4>(coord), glm::vec4(1, 1, 1, 1));


	view.ClearColor({Colours::white, 1.f});
	EXPECT_EQ(view.Get<glm::vec4>(C_TextureView::PixelCoordVec(0, 0)), glm::vec4(1, 1, 1, 1));
}

TEST_F(TextureView3x3Fixture, FillLine_Extremes)
{
	const auto& dim = storage.GetDimensions();
	view.FillLineSpan(Colours::red, 0, 0, dim.x + 4);
	EXPECT_EQ(view.Get<glm::vec3>(C_TextureView::PixelCoordVec{0, 0}), Colours::red);
	EXPECT_EQ(view.Get<glm::vec3>(C_TextureView::PixelCoordVec{5, 0}), Colours::red);

	view.FillLineSpan(Colours::green, dim.y - 1, 0, 5);
	EXPECT_EQ(view.Get<glm::vec3>(C_TextureView::PixelCoordVec{2, 2}), Colours::green);
	view.FillLineSpan(Colours::green, dim.y, 0, 5); // should not crash
}

TEST_F(TextureView3x3Fixture, ClampToEdge_Extremes)
{
	view.SetWrapFunction(E_WrapFunction::ClampToEdge);
	const auto&		 dim	   = storage.GetDimensions();
	const glm::uvec2 maxCoords = dim - glm::uvec2{1, 1};
	EXPECT_EQ(ClampCoordinates(glm::ivec2{5, 5}), maxCoords);
	EXPECT_EQ(ClampCoordinates(glm::ivec2{-5, -5}), glm::uvec2(0));
}

TEST_F(TextureView3x3Fixture, Repeat)
{
	view.SetWrapFunction(E_WrapFunction::Repeat);
	const auto&		 dim	   = storage.GetDimensions();
	const glm::uvec2 maxCoords = dim - glm::uvec2{1, 1};
	EXPECT_EQ(ClampCoordinates(dim), glm::uvec2(0));
	EXPECT_EQ(ClampCoordinates(glm::ivec2(-3, 0)), glm::uvec2(0, 0));
}

// ----- T_Nearest: both Sample variants -----
// UV pixel-centre formula (3x3): uv.x=(px+0.5)/3, uv.y=1-(py+0.5)/3
// Correct pixel coord via: px=uv.x*3-0.5, py=(1-uv.y)*3-0.5  (continuous, no floor)

TEST_F(TextureView3x3Fixture, Nearest_Vector_ExactColors)
{
	// pixel (x,y) linear index = y*3 + x  (3x3 storage)
	// UV centres: (0,0)->(1/6,5/6), (1,0)->(1/2,5/6), (1,1)->(1/2,1/2)
	view.ClearColor({Colours::black, 0.f});
	storage.SetPixel(glm::vec4(Colours::red, 0.f), 0);	 // pixel (0,0)
	storage.SetPixel(glm::vec4(Colours::green, 0.f), 1); // pixel (1,0)
	storage.SetPixel(glm::vec4(Colours::blue, 0.f), 4);	 // pixel (1,1)
	EXPECT_EQ((view.Sample<glm::vec3, T_Nearest>(glm::vec2(1.f / 6.f, 1.f / 6.f))), Colours::red);
	EXPECT_EQ((view.Sample<glm::vec3, T_Nearest>(glm::vec2(0.5f, 1.f / 6.f))), Colours::green);
	EXPECT_EQ((view.Sample<glm::vec3, T_Nearest>(glm::vec2(0.5f, 0.5f))), Colours::blue);
}

TEST_F(TextureView3x3Fixture, Nearest_SingleChannel_ExactValue)
{
	// Channel overload: GetPixelCoord maps UV to flipped pixel space.
	// UV(0.5,0.5) -> GetPixelCoord -> floor(1.5,1.5) = pixel(1,1) = index 4.
	view.ClearColor({Colours::black, 0.f});
	storage.SetPixel(glm::vec4(Colours::red, 0.f), 4); // pixel (1,1)
	EXPECT_EQ((view.Sample<float, T_Nearest>(glm::vec2(0.5f, 0.5f), E_TextureChannel::Red)), 1.f);
	EXPECT_EQ((view.Sample<float, T_Nearest>(glm::vec2(0.5f, 0.5f), E_TextureChannel::Green)), 0.f);
}

// ----- T_Bilinear: vector variant -----
// Pixel centres in UV for a 3x3: uv=(px+0.5)/3, uv.y=1-(py+0.5)/3
// Midpoints between centres (bilinear weight=0.5 in that axis):
//   (0,1)<->(1,1): uv=(1/3, 0.5) -> pixelCoord=(0.5,1.0), weights=(0.5,0)
//   (1,0)<->(1,1): uv=(0.5, 2/3) -> pixelCoord=(1.0,0.5), weights=(0,0.5)

TEST_F(TextureView3x3Fixture, Bilinear_Vector_AtPixelCenter_NoMixing)
{
	// UV(0.5,0.5) -> pixelCoord=(1.0,1.0), weights=fract(1,1)=(0,0) -> Q11=pixel(1,1)
	view.ClearColor({Colours::white, 0.f});
	storage.SetPixel(glm::vec4(Colours::red, 0.f), 4); // pixel (1,1)
	EXPECT_EQ((view.Sample<glm::vec3, T_Bilinear>(glm::vec2(0.5f, 0.5f))), Colours::red);
}

TEST_F(TextureView3x3Fixture, Bilinear_Vector_HalfwayHorizontal)
{
	// UV(1/3, 0.5) -> pixelCoord=(0.5, 1.0), weights=(0.5, 0), leftTopCoord=(0,1)
	// Q11=pixel(0,1)=red, Q21=pixel(1,1)=blue; R1=mix(red,blue,0.5)=(0.5,0,0.5)
	// weights.y=0 -> no vertical contribution
	view.ClearColor({Colours::black, 0.f});
	storage.SetPixel(glm::vec4(Colours::red, 0.f), 3);	// pixel (0,1) — index = 1*3+0 = 3
	storage.SetPixel(glm::vec4(Colours::blue, 0.f), 4); // pixel (1,1) — index = 1*3+1 = 4
	EXPECT_EQ((view.Sample<glm::vec3, T_Bilinear>(glm::vec2(1.f / 3.f, 0.5f))), glm::vec3(0.5f, 0.f, 0.5f));
}

TEST_F(TextureView3x3Fixture, Bilinear_Vector_HalfwayVertical)
{
	// UV(0.5, 2/3) -> pixelCoord=(1.0, 0.5), weights=(0, 0.5), leftTopCoord=(1,0)
	// Q11=pixel(1,0)=red, Q12=pixel(1,1)=blue; result=mix(red,blue,0.5)=(0.5,0,0.5)
	// weights.x=0 -> no horizontal contribution
	view.ClearColor({Colours::black, 0.f});
	storage.SetPixel(glm::vec4(Colours::red, 0.f), 1);	// pixel (1,0) — index = 0*3+1 = 1
	storage.SetPixel(glm::vec4(Colours::blue, 0.f), 4); // pixel (1,1) — index = 1*3+1 = 4
	EXPECT_EQ((view.Sample<glm::vec3, T_Bilinear>(glm::vec2(0.5f, 1.f / 3.f))), glm::vec3(0.5f, 0.f, 0.5f));
}

TEST_F(TextureView2x2Fixture, Bilinear_Vector_FourCornerMix)
{
	// UV(0.5,0.5): weights=(0.5,0.5) -> 25% each corner
	// 0.25*red + 0.25*green + 0.25*blue + 0.25*white = (0.5, 0.5, 0.5)
	storage.SetPixel(glm::vec4(Colours::red, 0.f), 0);	 // pixel (0,0)
	storage.SetPixel(glm::vec4(Colours::green, 0.f), 1); // pixel (1,0)
	storage.SetPixel(glm::vec4(Colours::blue, 0.f), 2);	 // pixel (0,1)
	storage.SetPixel(glm::vec4(Colours::white, 0.f), 3); // pixel (1,1)
	EXPECT_EQ((view.Sample<glm::vec3, T_Bilinear>(glm::vec2(0.5f, 0.5f))), glm::vec3(0.5f, 0.5f, 0.5f));
}

// ----- T_Bilinear: single-channel variant -----
// The channel overload calls GetPixelCoord which floors UV to integer pixel
// coords before passing to the filter, so bilinear weights are always (0,0).
// Bilinear through this path behaves identically to T_Nearest.

TEST_F(TextureView3x3Fixture, Bilinear_SingleChannel_MatchesNearestViaFlooredCoord)
{
	// UV(0.25,0.5) -> GetPixelCoord: floor(0.75,1.5)=pixel(0,1)=index 3=red
	// Bilinear weights=(0,0), result=exact pixel; same as T_Nearest.
	view.ClearColor({Colours::black, 0.f});
	storage.SetPixel(glm::vec4(Colours::red, 0.f), 3); // pixel (0,1)
	EXPECT_EQ((view.Sample<float, T_Bilinear>(glm::vec2(0.25f, 0.5f), E_TextureChannel::Red)), 1.f);
	EXPECT_EQ((view.Sample<float, T_Nearest>(glm::vec2(0.25f, 0.5f), E_TextureChannel::Red)), 1.f);
	EXPECT_EQ((view.Sample<float, T_Bilinear>(glm::vec2(0.25f, 0.5f), E_TextureChannel::Green)), 0.f);
}

// ----- T_Bilinear: border colour blending (2x2 fixture) -----
// Research note: per OpenGL §8.14 / Vulkan §15.2, out-of-bounds samples in bilinear
// filtering are replaced by the border colour before the weighted blend is computed.
// The border colour therefore contributes proportionally to the sample weights.
//
// 2x2 UV centres: (1,0) -> uv=(0.75, 0.75). Midpoints to the right column (x=1.5, 1.25
// in pixel space) map to uv.x=(1.5+0.5)/2=1.0 and uv.x=(1.25+0.5)/2=0.875 respectively.

TEST_F(TextureView2x2Fixture, Bilinear_BorderColor_HalfBlend)
{
	// UV(1.0, 0.75) -> pixelCoord=(1.5, 0.0), weights=(0.5, 0), leftTopCoord=(1,0)
	// Q11=pixel(1,0)=white(in), Q21=pixel(2,0)=border=black(out)
	// R1=mix(white,black,0.5)=(0.5,0.5,0.5); weights.y=0 -> result=R1
	view.ClearColor({Colours::white, 0.f});
	view.SetBorderColor(glm::vec4(0.f, 0.f, 0.f, 0.f));
	view.SetWrapFunction(E_WrapFunction::ClampToBorder);
	EXPECT_EQ((view.Sample<glm::vec3, T_Bilinear>(glm::vec2(1, 0.75f))), glm::vec3(0.5f, 0.5f, 0.5f));
}

TEST_F(TextureView2x2Fixture, Bilinear_BorderColor_LeftToPixelCenter)
{
	// we hit here right bottom pixel, but to the left from its center
	// 0.75 makes it to be in the center of the pixel
	view.ClearColor({Colours::white, 0.f});
	view.SetBorderColor(glm::vec4(0.f, 0.f, 0.f, 0.f));
	view.SetWrapFunction(E_WrapFunction::ClampToBorder);
	EXPECT_EQ((view.Sample<glm::vec3, T_Bilinear>(glm::vec2(5.f / 8.f, 0.75f))), Colours::white);
}

TEST_F(TextureView2x2Fixture, Bilinear_BorderColor_AbovePixelCenter)
{
	// we hit here right bottom pixel, but above its center
	// 0.75 makes it to be in the center of the pixel
	view.ClearColor({Colours::white, 0.f});
	view.SetBorderColor(glm::vec4(0.f, 0.f, 0.f, 0.f));
	view.SetWrapFunction(E_WrapFunction::ClampToBorder);
	EXPECT_EQ((view.Sample<glm::vec3, T_Bilinear>(glm::vec2(0.75f, 5.f / 8.f))), Colours::white);
}

TEST_F(TextureView2x2Fixture, Bilinear_BorderColor_HalfBlend_InRightTopPixelOfFilter)
{
	// UV(0.25f - 0.5f/4.f, 0.75) -> pixelCoord=(1.5, 0.0), weights=(0.5, 0), leftTopCoord=(1,0)
	// Q11=pixel(1,0)=white(in), Q21=pixel(2,0)=border=black(out)
	// R1=mix(white,black,0.5)=(0.5,0.5,0.5); weights.y=0 -> result=R1
	view.ClearColor({Colours::white, 0.f});
	view.SetBorderColor(glm::vec4(0.f, 0.f, 0.f, 0.f));
	view.SetWrapFunction(E_WrapFunction::ClampToBorder);
	EXPECT_EQ((view.Sample<glm::vec3, T_Bilinear>(glm::vec2(0.25f - 0.5f / 4.f, 0.75f))), glm::vec3(0.75, 0.75, 0.75));
}

TEST_F(TextureView2x2Fixture, Bilinear_BorderColor_QuarterBlend)
{
	// UV(0.875, 0.75) -> pixelCoord=(1.25, 0.0), weights=(0.25, 0), leftTopCoord=(1,0)
	// Q11=white(in), Q21=border=black(out): mix(white,black,0.25)=(0.75,0.75,0.75)
	view.ClearColor({Colours::white, 0.f});
	view.SetBorderColor(glm::vec4(0.f, 0.f, 0.f, 0.f));
	view.SetWrapFunction(E_WrapFunction::ClampToBorder);
	EXPECT_EQ((view.Sample<glm::vec3, T_Bilinear>(glm::vec2(0.875f, 0.75f))), glm::vec3(0.75f, 0.75f, 0.75f));
}

} // namespace GLEngine::Renderer