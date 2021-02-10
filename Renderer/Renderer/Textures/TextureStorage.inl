#pragma once

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
}

//=================================================================================
template <class internalFormat> C_TextureViewStorageCPU<internalFormat>::~C_TextureViewStorageCPU() = default;

//=================================================================================
template <class internalFormat> int C_TextureViewStorageCPU<internalFormat>::GetI(std::size_t position) const
{
	return static_cast<int>(m_Data[position]);
}

//=================================================================================
template <class internalFormat> double C_TextureViewStorageCPU<internalFormat>::GetD(std::size_t position) const
{
	return static_cast<double>(m_Data[position]);
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
	return static_cast<std::underlying_type_t<E_TextureChannel>>(element);
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

} // namespace GLEngine::Renderer