#pragma once

#include <DULib/BitField.h>

namespace GLEngine::Core {
enum class E_KeyModifiers : std::uint8_t
{
	Shift	 = 1,
	Control	 = 2,
	Super	 = 4,
	CapsLock = 8,
	NumLock	 = 16,
};
} // namespace GLEngine::Core

//=================================================================================
template <> struct DULib::enable_BitField_operators<GLEngine::Core::E_KeyModifiers> {
	static constexpr bool enable = true;
};

template <> struct DULib::BitField_UsedBitsCounter<GLEngine::Core::E_KeyModifiers> {
	static constexpr std::size_t usedBits = 5;
};
