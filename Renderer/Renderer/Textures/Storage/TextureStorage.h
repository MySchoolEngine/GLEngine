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

	/**
	 * Fills line span including start and end pixel. Allows for performance improvement in linear storages.
	 */
	virtual void FillLineSpan(const glm::vec3& colour, unsigned int line, unsigned int start, unsigned int end) = 0;

protected:
	virtual void SetInternal(double value, std::size_t position) = 0;
	virtual void SetInternal(int value, std::size_t position)	 = 0;

	[[nodiscard]] glm::vec4 Swizzle(const glm::vec4& value) const;

	glm::uvec2 m_Dimensions;
	T_Channels m_Channels;
};
} // namespace GLEngine::Renderer
