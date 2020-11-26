#pragma once

/**
 * This class serves as CPU side view to the texture independently whether lies on
 * CPU or GPU memory.
 * 
 * Problem breaks up into three sub-problems:
 * 1] Storage (cpu, gpu, fixed, etc)
 * 2] Internal format
 * 3] Conversion into external format
 * 
 * Storage
 * We can store texture either on CPU or GPU side.
 * GPU - we can either download data or access them directly.
 * CPU - we can use some STD container
 * 
 * Internal format
 * Formats differs in three orthogonal axes. 
 * 1] Number of elements
 * 2] Precision
 * 3] Floating point integer formats
 * 
 * Conversion to external format
 * Mostly sRGB to integral
 * 
 * 
 * 
 * Let's start with simple part CPU side let's say GL_RGB8 format 
 */

namespace GLEngine::Renderer
{
template<class internalFormat, int elements>
class I_TextureViewStorage
{
public:
	I_TextureViewStorage(std::size_t width, std::size_t height);
	virtual internalFormat& Get(std::size_t position) const = 0;
	virtual internalFormat& Get(std::size_t position) = 0;
};
template<class internalFormat, int elements>
class C_TextureViewStorageCPU
{
public:
	C_TextureViewStorageCPU(std::size_t width, std::size_t height);
	virtual const internalFormat& Get(std::size_t position) const override;
	virtual internalFormat& Get(std::size_t position) override;
private:
	std::vector<internalFormat> m_Data;
};

template<class internalFormat, int elements, std::size_t width, std::size_t height>
class C_TextureViewStorageCPUConst : I_TextureViewStorage
{
public:
	constexpr C_TextureViewStorageCPUConst()
		: I_TextureViewStorage(width, height) {}
private:
	std::array<internalFormat, width* height* elements> m_Data;
};

class C_TextureView
{
	template<class internalFormat>
	C_TextureView(std::size_t width, std::size_t height);


};
}
