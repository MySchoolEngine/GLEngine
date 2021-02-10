#pragma once

#include <Renderer/RendererApi.h>

namespace GLEngine::Renderer {
//=================================================================================
enum class E_TextureChannel
{
	Red,
	Green,
	Blue,
	Alpha,
	None,
};

enum class E_TextureTypes
{
	IntegralNormalized, //[ 0,1]
	Integral,
	SignedNormalized, //[-1,1]
	Signed,
	Floating,
};

using T_Channels = std::array<E_TextureChannel, 4>;

//=================================================================================
class RENDERER_API_EXPORT I_TextureViewStorage {
public:
	I_TextureViewStorage(std::size_t width, std::size_t height);
	virtual ~I_TextureViewStorage();
	[[nodiscard]] virtual float GetF(std::size_t position) const = 0;
	[[nodiscard]] virtual int	GetI(std::size_t position) const = 0;

	void Set(double value, std::size_t position);
	void Set(int value, std::size_t position);

	[[nodiscard]] virtual const void* GetData() const							 = 0;
	[[nodiscard]] virtual void*		  GetData()									 = 0;
	virtual const void				  SetData(const void* data, std::size_t len) = 0;

	[[nodiscard]] virtual std::uint8_t GetNumElements() const							= 0;
	[[nodiscard]] virtual std::uint8_t GetChannelOffset(E_TextureChannel element) const = 0;

	[[nodiscard]] glm::ivec2 GetDimensions() const;

	// defines how channels are laid out in memory
	[[nodiscard]] T_Channels GetChannels() const { return m_Channels; }
	void					 SetChannels(T_Channels swizzle) { m_Channels = swizzle; }
	[[nodiscard]] bool		 IsSwizzled() const;

	[[nodiscard]] virtual E_TextureTypes GetStorageType() const = 0;

protected:
	virtual void SetInternal(double value, std::size_t position) = 0;
	virtual void SetInternal(int value, std::size_t position)	 = 0;

	glm::ivec2 m_Dimensions;
	T_Channels m_Channels;
};

//=================================================================================
template <class internalFormat> class C_TextureViewStorageCPU : public I_TextureViewStorage {
public:
	C_TextureViewStorageCPU(std::size_t width, std::size_t height, std::uint8_t elements);
	virtual ~C_TextureViewStorageCPU();

	[[nodiscard]] virtual float GetF(std::size_t position) const override;
	[[nodiscard]] virtual int	GetI(std::size_t position) const override;

	[[nodiscard]] virtual std::uint8_t GetNumElements() const override;

	[[nodiscard]] virtual const void* GetData() const override;
	[[nodiscard]] virtual void*		  GetData() override;
	virtual const void				  SetData(const void* data, std::size_t pixels) override;

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
