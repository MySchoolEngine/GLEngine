#pragma once

#include <Core/CoreMacros.h>

#include <Utils/CompileTime/CompileTimeUtils.h>

namespace GLEngine::Renderer {
//=================================================================================
// C_TextureViewStorageCPU<type, int>
//=================================================================================
template <class internalFormat>
C_TextureViewStorageCPU<internalFormat>::C_TextureViewStorageCPU(std::size_t width, std::size_t height, std::uint8_t elements)
	: I_TextureViewStorage(width, height)
	, m_Elements(elements)
{
	m_Data.resize(width * height * elements);
	SetChannels(Renderer::GetOrderedChannels(elements));
}

//=================================================================================
template <class internalFormat> C_TextureViewStorageCPU<internalFormat>::~C_TextureViewStorageCPU() = default;

//=================================================================================
template <class internalFormat> int C_TextureViewStorageCPU<internalFormat>::GetI(std::size_t position) const
{
	return static_cast<int>(m_Data[position]);
}

//=================================================================================
template <class internalFormat> float C_TextureViewStorageCPU<internalFormat>::GetF(std::size_t position) const
{
	return static_cast<float>(m_Data[position]);
}

//=================================================================================
template <class internalFormat> std::uint8_t C_TextureViewStorageCPU<internalFormat>::GetNumElements() const
{
	return m_Elements;
}

//=================================================================================
template <class internalFormat> const void* C_TextureViewStorageCPU<internalFormat>::GetData() const
{
	return m_Data.data();
}

//=================================================================================
template <class internalFormat> void* C_TextureViewStorageCPU<internalFormat>::GetData()
{
	return m_Data.data();
}

//=================================================================================
template <class internalFormat> const void C_TextureViewStorageCPU<internalFormat>::SetData(const void* data, std::size_t pixels)
{
	GLE_ASSERT(pixels == m_Data.size() / m_Elements, "Larger image than buffer");
	memcpy(m_Data.data(), data, pixels * sizeof(internalFormat) * m_Elements);
}

//=================================================================================
template <class internalFormat> std::uint8_t C_TextureViewStorageCPU<internalFormat>::GetChannelOffset(E_TextureChannel element) const
{
	const auto it = std::find(m_Channels.begin(), m_Channels.end(), element);
	if (it != m_Channels.end())
	{
		return static_cast<std::uint8_t>(std::distance(m_Channels.begin(), it));
	}
	return -1; // wrong
}

//=================================================================================
template <class internalFormat> void C_TextureViewStorageCPU<internalFormat>::SetInternal(int value, std::size_t position)
{
	m_Data[position] = static_cast<internalFormat>(value);
}

//=================================================================================
template <class internalFormat> void C_TextureViewStorageCPU<internalFormat>::SetInternal(double value, std::size_t position)
{
	m_Data[position] = static_cast<internalFormat>(value);
}

//=================================================================================
template <class internalFormat> E_TextureTypes C_TextureViewStorageCPU<internalFormat>::GetStorageType() const
{
	if (std::is_same_v<internalFormat, std::uint8_t>)
	{
		return E_TextureTypes::IntegralNormalized;
	}
	if (std::is_same_v<internalFormat, std::int8_t>)
	{
		return E_TextureTypes::Integral;
	}
	if (std::is_same_v<internalFormat, float>)
	{
		return E_TextureTypes::Floating;
	}

	static_assert(T_ContainsType_v<internalFormat, std::uint8_t, std::int8_t, float>, "Unsupported internal type of texture storage.");
}

//=================================================================================
template <class internalFormat> inline void C_TextureViewStorageCPU<internalFormat>::SetAll(const glm::vec4& value)
{
	glm::vec<4, internalFormat, glm::defaultp> realValue(Swizzle(value));
	for (auto it = m_Data.begin(); it != m_Data.end(); it += m_Elements)
	{
		std::copy_n(static_cast<const internalFormat*>(&realValue.x), m_Elements, it);
	}
}

//=================================================================================
template <class internalFormat> void C_TextureViewStorageCPU<internalFormat>::SetPixel(const glm::vec4& value, std::size_t position)
{
	glm::vec<4, internalFormat, glm::defaultp> realValue(Swizzle(value));
	auto									   it = m_Data.begin();
	std::advance(it, position * m_Elements);
	std::copy_n(static_cast<const internalFormat*>(&realValue.x), m_Elements, it);
}

//=================================================================================
template <class internalFormat> inline void C_TextureViewStorageCPU<internalFormat>::FillLineSpan(const glm::vec3& colour, unsigned int line, unsigned int start, unsigned int end)
{
	glm::vec<4, internalFormat, glm::defaultp> realValue(Swizzle(glm::vec4{colour, 1.f}));
	auto									   it		= m_Data.begin();
	auto									   position = line * GetDimensions().x + start;
	std::advance(it, position * m_Elements);
	for (unsigned int i = start; i <= end; ++i, it += m_Elements)
	{
		std::copy_n(static_cast<const internalFormat*>(&realValue.x), m_Elements, it);
	}
}

//=================================================================================
template <class internalFormat> glm::vec4 C_TextureViewStorageCPU<internalFormat>::GetPixel(std::size_t pixelIndex) const
{
	glm::vec<4, internalFormat, glm::defaultp> value(0);
	memcpy(&value.x, &(m_Data[pixelIndex * m_Elements]), sizeof(internalFormat) * m_Elements);
	return glm::vec4(value);
}

} // namespace GLEngine::Renderer
