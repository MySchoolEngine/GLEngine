#pragma once

#include <Renderer/RendererApi.h>

namespace GLEngine::Renderer {
//=================================================================================
enum class E_TextureElement
{
	Red,
	Green,
	Blue,
	Alpha,
};

enum class E_TextureTypes
{
	IntegralNormalized,	//[ 0,1]
	Integral,
	SignedNormalized,		//[-1,1]
	Signed,
	Floating,
};

//=================================================================================
class RENDERER_API_EXPORT I_TextureViewStorage
{
public:
	I_TextureViewStorage(std::size_t width, std::size_t height);
	virtual ~I_TextureViewStorage();
	[[nodiscard]] virtual double GetD(std::size_t position) const = 0;
	[[nodiscard]] virtual int GetI(std::size_t position) const = 0;

	void Set(double value, std::size_t position);
	void Set(int value, std::size_t position);

	[[nodiscard]] virtual const void* GetData() const = 0;
	virtual const void SetData(const void* data, std::size_t len) = 0;

	[[nodiscard]] virtual std::uint8_t GetNumElements() const = 0;
	[[nodiscard]] virtual std::uint8_t GetElementOffset(E_TextureElement element) const = 0;

	[[nodiscard]] glm::ivec2 GetDimensions() const;
protected:
	virtual void SetInternal(double value, std::size_t position) = 0;
	virtual void SetInternal(int value, std::size_t position) = 0;

	glm::ivec2	m_Dimensions;
};

//=================================================================================
template<class internalFormat, int elements>
class C_TextureViewStorageCPU : public I_TextureViewStorage
{
public:
	C_TextureViewStorageCPU(std::size_t width, std::size_t height);
	virtual ~C_TextureViewStorageCPU();

	[[nodiscard]] virtual double GetD(std::size_t position) const override;
	[[nodiscard]] virtual int GetI(std::size_t position) const override;

	[[nodiscard]] virtual constexpr std::uint8_t GetNumElements() const;

	[[nodiscard]] virtual const void* GetData() const;
	virtual const void SetData(const void* data, std::size_t pixels);

	// todo allow swizzle
	[[nodiscard]] virtual std::uint8_t GetElementOffset(E_TextureElement element) const;
protected:
	virtual void SetInternal(double value, std::size_t position) override;
	virtual void SetInternal(int value, std::size_t position) override;
private:
	std::vector<internalFormat>	m_Data;
};

//=================================================================================

//=================================================================================
//template<class internalFormat, int elements, std::size_t width, std::size_t height>
//class C_TextureViewStorageCPUConst : I_TextureViewStorage
//{
//public:
//	constexpr C_TextureViewStorageCPUConst()
//		: I_TextureViewStorage(width, height) {}
//private:
//	std::array<internalFormat, width* height* elements> m_Data;
//};
}
