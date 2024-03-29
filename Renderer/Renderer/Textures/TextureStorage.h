#pragma once

#include <Renderer/RendererApi.h>
#include <Renderer/Textures/TextureDefinitions.h>

namespace GLEngine::Renderer {

//=================================================================================
class RENDERER_API_EXPORT I_TextureViewStorage {
public:
	I_TextureViewStorage(std::size_t width, std::size_t height);
	virtual ~I_TextureViewStorage();
	/**
	 * @param	position	element position in row by row and element after element
	 * @returns	float		value on given position
	 */
	[[nodiscard]] virtual float GetF(std::size_t position) const = 0;
	/**
	 * @param	position	element position in row by row and element after element
	 * @returns	int			value on given position
	 */
	[[nodiscard]] virtual int GetI(std::size_t position) const = 0;
	/**
	 * @param	pixelIndex	element position in row by row indexed by whole pixels. @todo should be swizzled
	 * @returns	float		value on given position. Unused elements are not defined
	 */
	[[nodiscard]] virtual glm::vec4 GetPixel(std::size_t pixelIndex) const = 0;

	/**
	 * @param	value		value to be stored
	 * @param	position	element position in row by row and channel after channel.
	 */
	void Set(double value, std::size_t position);
	/**
	 * @param	value		value to be stored
	 * @param	position	element position in row by row and channel after channel.
	 */
	void Set(int value, std::size_t position);
	/**
	 * @param	value		value to be saved. In RGBA format. This will be internally
	 *						swizzled to correct channel order
	 * @param	position	pixel index in row by row linear array
	 */
	virtual void SetPixel(const glm::vec4& value, std::size_t position) = 0;
	/**
	 * Sets same value to all pixels. Basically clear colour buffer
	 * @param	value		value to be stored
	 */
	virtual void SetAll(const glm::vec4& value) = 0;

	[[nodiscard]] virtual const void* GetData() const							 = 0;
	[[nodiscard]] virtual void*		  GetData()									 = 0;
	virtual const void				  SetData(const void* data, std::size_t len) = 0;

	/**
	 * @returns	uint8_t	Number of channels
	 */
	[[nodiscard]] virtual std::uint8_t GetNumElements() const							= 0;
	[[nodiscard]] virtual std::uint8_t GetChannelOffset(E_TextureChannel element) const = 0;

	[[nodiscard]] glm::uvec2 GetDimensions() const;

	/************************************************************************/
	/* Swizzle                                                              */
	/************************************************************************/
	/**
	 * @returns	T_Channels	Channel ordering in memory.
	 */
	[[nodiscard]] T_Channels GetChannels() const { return m_Channels; }
	/**
	 * Sets the ordering of channels in memory. Each colour channel should have unique index.
	 */
	void SetChannels(T_Channels swizzle);
	/**
	 * @returns	true	if ordered RGBA with trailing channels possible missing. false otherwise
	 */
	[[nodiscard]] bool IsSwizzled() const;

	[[nodiscard]] virtual E_TextureTypes GetStorageType() const = 0;

protected:
	virtual void SetInternal(double value, std::size_t position) = 0;
	virtual void SetInternal(int value, std::size_t position)	 = 0;

	[[nodiscard]] glm::vec4 Swizzle(const glm::vec4& value) const;

	glm::uvec2 m_Dimensions;
	T_Channels m_Channels;
};

//=================================================================================
template <class internalFormat> class C_TextureViewStorageCPU : public I_TextureViewStorage {
public:
	C_TextureViewStorageCPU(std::size_t width, std::size_t height, std::uint8_t elements);
	virtual ~C_TextureViewStorageCPU();

	[[nodiscard]] virtual float		GetF(std::size_t position) const override;
	[[nodiscard]] virtual int		GetI(std::size_t position) const override;
	[[nodiscard]] virtual glm::vec4 GetPixel(std::size_t pixelIndex) const override;

	virtual void SetPixel(const glm::vec4& value, std::size_t position) override;

	/**
	 * @return	Number of channels per pixel
	 */
	[[nodiscard]] virtual std::uint8_t GetNumElements() const override;

	[[nodiscard]] virtual const void* GetData() const override;
	[[nodiscard]] virtual void*		  GetData() override;
	virtual const void				  SetData(const void* data, std::size_t pixels) override;
	void							  SetAll(const glm::vec4& value) override;

	// todo allow swizzle
	[[nodiscard]] virtual std::uint8_t	 GetChannelOffset(E_TextureChannel element) const override;
	[[nodiscard]] virtual E_TextureTypes GetStorageType() const override;

protected:
	virtual void SetInternal(double value, std::size_t position) override;
	virtual void SetInternal(int value, std::size_t position) override;

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
#include <Renderer/Textures/TextureStorage.inl>
