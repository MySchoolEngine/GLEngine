#include <RendererStdafx.h>

#include <Renderer/Textures/TextureStorage.h>

namespace GLEngine::Renderer {

//=================================================================================
// I_TextureViewStorage
//=================================================================================
I_TextureViewStorage::I_TextureViewStorage(std::size_t width, std::size_t height) 
	: m_Dimensions({ width, height })
{

}

//=================================================================================
I_TextureViewStorage::~I_TextureViewStorage() = default;

//=================================================================================
void I_TextureViewStorage::Set(double value, std::size_t position)
{
	if (position >= static_cast<std::size_t>(m_Dimensions.x) * m_Dimensions.y * GetNumElements())
	{
		CORE_LOG(E_Level::Info, E_Context::Render, "Writing outside of texture buffer. Result would be discarded.");
		return;
	}
	SetInternal(value, position);
}

//=================================================================================
void I_TextureViewStorage::Set(int value, std::size_t position)
{
	if (position >= static_cast<std::size_t>(m_Dimensions.x) * m_Dimensions.y * GetNumElements())
	{
		CORE_LOG(E_Level::Info, E_Context::Render, "Writing outside of texture buffer. Result would be discarded.");
		return;
	}
	SetInternal(value, position);
}

//=================================================================================
glm::ivec2 I_TextureViewStorage::GetDimensions() const
{
	return m_Dimensions;
}

//=================================================================================
// C_TextureViewStorageCPU<type, int>
//=================================================================================
template<class internalFormat, int elements>
C_TextureViewStorageCPU<internalFormat, elements>::C_TextureViewStorageCPU(std::size_t width, std::size_t height) 
	: I_TextureViewStorage(width, height)
{
	m_Data.resize(width * height * elements);
}

//=================================================================================
template<class internalFormat, int elements>
C_TextureViewStorageCPU<internalFormat, elements>::~C_TextureViewStorageCPU() = default;

//=================================================================================
template<class internalFormat, int elements>
int C_TextureViewStorageCPU<internalFormat, elements>::GetI(std::size_t position) const
{
	return static_cast<int>(m_Data[position]);
}

//=================================================================================
template<class internalFormat, int elements>
double C_TextureViewStorageCPU<internalFormat, elements>::GetD(std::size_t position) const
{
	return static_cast<double>(m_Data[position]);
}

//=================================================================================
template<class internalFormat, int elements>
constexpr std::uint8_t C_TextureViewStorageCPU<internalFormat, elements>::GetNumElements() const
{
	return elements;
}

//=================================================================================
template<class internalFormat, int elements>
const void* C_TextureViewStorageCPU<internalFormat, elements>::GetData() const
{
	return m_Data.data();
}

//=================================================================================
template<class internalFormat, int elements>
const void C_TextureViewStorageCPU<internalFormat, elements>::SetData(const void* data, std::size_t pixels)
{
	GLE_ASSERT(pixels == m_Data.size() / elements, "Larger image than buffer");
	memcpy(m_Data.data(), data, pixels * sizeof(internalFormat) * elements);
}

//=================================================================================
template<class internalFormat, int elements>
std::uint8_t C_TextureViewStorageCPU<internalFormat, elements>::GetElementOffset(E_TextureElement element) const
{
	return static_cast<std::underlying_type_t<E_TextureElement>>(element);
}

//=================================================================================
template<class internalFormat, int elements>
void C_TextureViewStorageCPU<internalFormat, elements>::SetInternal(int value, std::size_t position)
{
	m_Data[position] = static_cast<internalFormat>(value);
}

//=================================================================================
template<class internalFormat, int elements>
void C_TextureViewStorageCPU<internalFormat, elements>::SetInternal(double value, std::size_t position)
{
	m_Data[position] = static_cast<internalFormat>(value);
}

}