#include <gtest/gtest.h>

#include <Renderer/Colours.h>
#include <Renderer/Textures/Storage/TextureLinearStorage.h>
#include <Renderer/Textures/Storage/TextureStorage.h>

#include <RendererTest/GlmPrinters.h>

namespace GLEngine::Renderer {

class TextureStorageFixture : public ::testing::Test {
public:
	inline const static T_Channels RGBAChannels = T_Channels{E_TextureChannel::Red, E_TextureChannel::Green, E_TextureChannel::Blue, E_TextureChannel::Alpha};
	inline const static T_Channels BGRAChannels = T_Channels{E_TextureChannel::Blue, E_TextureChannel::Green, E_TextureChannel::Red, E_TextureChannel::Alpha};
};
TEST_F(TextureStorageFixture, Constructor)
{
	C_TextureViewStorageCPU<std::uint8_t> storage3(1, 1, 3);
	EXPECT_EQ(storage3.GetDimensions(), glm::uvec2(1, 1));
	EXPECT_EQ(storage3.GetChannels()[3], E_TextureChannel::None); // should be set to None, as it is unused
	EXPECT_EQ(storage3.GetNumElements(), 3);

	C_TextureViewStorageCPU<std::uint8_t> storage4(1, 1, 4);
	EXPECT_EQ(storage4.GetChannels(), RGBAChannels); // should be set to None, as it is unused
	EXPECT_EQ(storage4.GetNumElements(), 4);
}
TEST_F(TextureStorageFixture, IsSwizzled)
{
	C_TextureViewStorageCPU<std::uint8_t> storage(1, 1, 3);
	storage.SetPixel({0.f, 1.f, 2.f, 3.f}, 0);
	EXPECT_FALSE(storage.IsSwizzled()); // by default no swizzle
	storage.SetChannels({E_TextureChannel::Red, E_TextureChannel::Red, E_TextureChannel::Red, E_TextureChannel::None});
	EXPECT_TRUE(storage.IsSwizzled()); // by default no swizzle
}

TEST_F(TextureStorageFixture, SetDouble)
{
	C_TextureViewStorageCPU<float> storage(2, 2, 3);
	storage.Set(1.0, 7); // green channel of bottom left pixel
	EXPECT_EQ(storage.GetPixel(2).g, 1.0);
}

TEST_F(TextureStorageFixture, GetChannelOffset)
{
	C_TextureViewStorageCPU<float> storage(2, 2, 4);
	EXPECT_EQ(storage.GetChannelOffset(E_TextureChannel::Red), 0);
	EXPECT_EQ(storage.GetChannelOffset(E_TextureChannel::Green), 1);
	EXPECT_EQ(storage.GetChannelOffset(E_TextureChannel::Blue), 2);
	EXPECT_EQ(storage.GetChannelOffset(E_TextureChannel::Alpha), 3);

	storage.SetChannels(BGRAChannels);
	EXPECT_EQ(storage.GetChannelOffset(E_TextureChannel::Red), 2);
	EXPECT_EQ(storage.GetChannelOffset(E_TextureChannel::Green), 1);
	EXPECT_EQ(storage.GetChannelOffset(E_TextureChannel::Blue), 0);
	EXPECT_EQ(storage.GetChannelOffset(E_TextureChannel::None), static_cast<std::uint8_t>(-1));
	EXPECT_EQ(storage.GetChannelOffset(E_TextureChannel::Alpha), 3);
}

TEST_F(TextureStorageFixture, GetPixel)
{
	C_TextureViewStorageCPU<float> storage(2, 2, 3);
	storage.Set(1.0, 6); // red channel of bottom left pixel
	EXPECT_EQ(storage.GetPixel(2), glm::vec4(1.0, .0, 0.0, 0.0));
	storage.SetChannels(BGRAChannels);
	// EXPECT_EQ(storage.GetPixel(3), glm::vec4(0.0, .0, 1.0, 0.0)); unsure about correctness
}

TEST_F(TextureStorageFixture, SetAll)
{
}
} // namespace GLEngine::Renderer