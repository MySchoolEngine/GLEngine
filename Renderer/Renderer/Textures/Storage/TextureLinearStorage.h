#pragma once

#include <Renderer/Textures/TextureDefinitions.h>
#include <Renderer/Textures/Storage/TextureStorage.h>

namespace GLEngine::Renderer {
//=================================================================================
template <class internalFormat> class C_TextureViewStorageCPU : public I_TextureViewStorage {
public:
	C_TextureViewStorageCPU(std::size_t width, std::size_t height, std::uint8_t elements);
	~C_TextureViewStorageCPU() override;

	[[nodiscard]] float		GetF(std::size_t position) const override;
	[[nodiscard]] int		GetI(std::size_t position) const override;
	[[nodiscard]] glm::vec4 GetPixel(std::size_t pixelIndex) const override;

	void SetPixel(const glm::vec4& value, std::size_t position) override;

	/**
	 * @return	Number of channels per pixel
	 */
	[[nodiscard]] std::uint8_t GetNumElements() const override;

	[[nodiscard]] const void* GetData() const override;
	[[nodiscard]] void*		  GetData() override;
	const void				  SetData(const void* data, std::size_t pixels) override;
	void							  SetAll(const glm::vec4& value) override;

	// todo allow swizzle
	[[nodiscard]] std::uint8_t	 GetChannelOffset(E_TextureChannel element) const override;
	[[nodiscard]] E_TextureTypes GetStorageType() const override;

	/**
	 * Fills line span including start and end pixel
	 */
	void FillLineSpan(const glm::vec3& colour, unsigned int line, unsigned int start, unsigned int end) override;

protected:
	void SetInternal(double value, std::size_t position) override;
	void SetInternal(int value, std::size_t position) override;

private:
	std::vector<internalFormat> m_Data;
	std::uint8_t				m_Elements;
};

//=================================================================================

//=================================================================================
// template<class internalFormat, int elements, std::size_t width, std::size_t height>
// class C_TextureViewStorageCPUConst : I_TextureViewStorage
//{
// public:
//	constexpr C_TextureViewStorageCPUConst()
//		: I_TextureViewStorage(width, height) {}
// private:
//	std::array<internalFormat, width* height* elements> m_Data;
//};
} // namespace GLEngine::Renderer

#include <Renderer/Textures/Storage/TextureLinearStorage.inl>