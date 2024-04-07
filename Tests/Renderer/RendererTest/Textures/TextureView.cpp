#include <gtest/gtest.h>

#include <Renderer/Colours.h>
#include <Renderer/Textures/Filtering/LinearFiltering.h>
#include <Renderer/Textures/Storage/TextureLinearStorage.h>
#include <Renderer/Textures/TextureView.h>

#include <RendererTest/GlmPrinters.h>

namespace GLEngine::Renderer {
TEST(TextureView, Border)
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
} // namespace GLEngine::Renderer