#pragma once

#include <Renderer/Textures/Filtering/LinearFiltering.h>

#include <Utils/EnumUtils.h>

namespace GLEngine::Renderer {

//=================================================================================
template <class T, class Filter /*= T_Nearest*/> [[nodiscard]] T C_TextureView::Get(const glm::vec2& uv, E_TextureChannel element) const
{
	const auto pixelCoord = GetPixelCoord(uv);
	return Filter().template FilteredGet<T>(pixelCoord, *this, element);
}

//=================================================================================
template <> inline std::uint8_t C_TextureView::Get<std::uint8_t>(const glm::ivec2& uv, E_TextureChannel element) const
{
	glm::ivec2 coord = uv;
	if (IsOutsideBorders(uv))
		if (UseBorderColor())
			return static_cast<std::uint8_t>(GetBorderColor<glm::ivec4>()[::Utils::ToIndex(element)]);
		else
			coord = ClampCoordinates(uv);

	// TODO: only this line needs to be in specific template
	return m_Storage->GetI(GetAddress(coord) + m_Storage->GetChannelOffset(element));
}

//=================================================================================
template <> inline float C_TextureView::Get<float>(const glm::ivec2& uv, E_TextureChannel element) const
{
	glm::ivec2 coord = uv;
	if (IsOutsideBorders(uv))
		if (UseBorderColor())
			return GetBorderColor<glm::vec4>()[::Utils::ToIndex(element)];
		else
			coord = ClampCoordinates(uv);

	// TODO: only this line needs to be in specific template
	return m_Storage->GetF(GetAddress(coord) + m_Storage->GetChannelOffset(element));
}

//=================================================================================
template <class T, typename /*= std::enable_if_t<glm::type<T>::is_vec>*/> T C_TextureView::Get(const glm::ivec2& uv) const
{
	glm::ivec2 coord = uv;
	if (IsOutsideBorders(uv))
		if (UseBorderColor())
			return GetBorderColor<T>();
		else
			coord = ClampCoordinates(uv);

	return m_Storage->GetPixel(GetPixelAddress(coord));
}

//=================================================================================
template <class T, class Filter, typename /*= std::enable_if_t<glm::type<T>::is_vec>*/> T C_TextureView::Get(const glm::vec2& uv) const
{
	// TODO: border colour could be handled simpler
	T ret;
	for (std::uint8_t i = 0; i < std::min(static_cast<std::uint8_t>(glm::type<T>::components), m_Storage->GetNumElements()); ++i)
	{
		ret[i] = Get<typename T::value_type, Filter>(uv, static_cast<E_TextureChannel>(i));
	}
	return ret;
}

//=================================================================================
template <> inline glm::vec4 C_TextureView::GetBorderColor() const
{
	return m_BorderColor;
}

//=================================================================================
template <> inline glm::ivec4 C_TextureView::GetBorderColor() const
{
	return glm::ivec4(m_BorderColor * 255.f);
}

//=================================================================================
template <> inline glm::vec3 C_TextureView::GetBorderColor() const
{
	return glm::vec3(m_BorderColor);
}

//=================================================================================
template <class T> void C_TextureView::Set(const glm::ivec2& uv, const T val, E_TextureChannel element)
{
	const auto dim = m_Storage->GetDimensions();
	if (uv.x < 0 || uv.x > dim.x || uv.y < 0 || uv.y > dim.y)
	{
		CORE_LOG(E_Level::Info, E_Context::Render, "Writing outside of texture buffer. Result would be discarded.");
		return;
	}

	m_Storage->Set(val, GetAddress(uv) + m_Storage->GetChannelOffset(element));
}

//=================================================================================
template <class T, typename /*= std::enable_if_t<glm::type<T>::is_vec>*/> void C_TextureView::Set(const glm::ivec2& uv, T&& val)
{
	// swizzle on views side
	if constexpr (std::is_same_v<std::remove_cv_t<T>, glm::vec4>)
	{
		m_Storage->SetPixel(val, GetPixelAddress(uv));
	}
	else if constexpr (std::is_same_v<std::remove_cv_t<T>, glm::vec3>)
	{
		m_Storage->SetPixel(glm::vec4(val, 1.f), GetPixelAddress(uv));
	}
	else
	{
		for (std::uint8_t i = 0; i < std::min(static_cast<std::uint8_t>(glm::type<T>::components), m_Storage->GetNumElements()); ++i)
		{
			Set(uv, val[i], static_cast<E_TextureChannel>(i));
		}
	}
}

//=================================================================================
//=================================================================================
//=================================================================================
inline glm::vec2 signNotZero(glm::vec2 v)
{
	return glm::vec2((v.x >= 0.0) ? +1.0 : -1.0, (v.y >= 0.0) ? +1.0 : -1.0);
}

inline glm::vec2 float32x3_to_oct(glm::vec3 v)
{
	// Project the sphere onto the octahedron, and then onto the xy plane
	glm::vec2 p = glm::vec2{v.x, v.y} * (1.0f / (abs(v.x) + abs(v.y) + abs(v.z)));
	// Reflect the folds of the lower hemisphere over the diagonals
	return (v.z <= 0.0f) ? ((1.0f - abs(glm::vec2{p.y, p.x})) * signNotZero(p)) : p;
}

inline glm::vec3 oct_to_float32x3(glm::vec2 e)
{
	glm::vec3 v = glm::vec3(e.x, e.y, 1.0f - abs(e.x) - abs(e.y));
	if (v.z < 0.0f)
	{
		auto r = (1.0f - glm::abs(glm::vec2{v.y, v.x})) * signNotZero({v.x, v.y});
		v.x	   = r.x;
		v.y	   = r.y;
	}
	return glm::normalize(v);
}

//=================================================================================
// C_OctahedralTextureView
//=================================================================================
template <class T> void C_OctahedralTextureView::Set(const glm::vec3& direction, const T val, E_TextureChannel element)
{
	const auto coord	 = float32x3_to_oct(direction);
	const auto realCoord = glm::ivec2{static_cast<float>(m_Size) * (coord / 2.0f + 0.5f)};

	m_View.Set(realCoord, val, element);
}

} // namespace GLEngine::Renderer