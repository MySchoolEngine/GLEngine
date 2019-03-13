#pragma once

#include <type_traits>

namespace Utils {
template<class Enum, typename = std::enable_if<std::is_enum<typename Enum>::value>::type>
class C_BitField {
	using value_type = typename std::underlying_type<Enum>::type;
public:
	template<typename = std::is_integral<typename value_type>::type>
	C_BitField() 
		: m_Flags(0){}

	C_BitField(const Enum bit)
		: m_Flags(static_cast<value_type>(bit)){}

	C_BitField(const C_BitField& other) {
		m_Flags = other.m_Flags;
	}

	C_BitField(const C_BitField&& other) {
		m_Flags = other.m_Flags;
	}

	~C_BitField() = default;

	bool CheckFlag(const Enum flag) {
		return !!(static_cast<value_type>(flag)&m_Flags);
	}

	void SetFlag(const Enum flag) {
		m_Flags |= static_cast<value_type>(flag);
	}

	void SetFlags(const C_BitField& field) {
		m_Flags &= field.GetFlags();
	}

	void ClearFlag(const Enum flag) {
		m_Flags &= ~static_cast<value_type>(flag);
	}

	void ToggleFlag(const Enum flag) {
		m_Flags ^= static_cast<value_type>(flag);
	}

	/*C_BitField operator|(const Enum bit) const {
		C_BitField ret(this);
		ret.SetFlag(bit);
		return ret;
	}*/

	C_BitField& operator|=(const Enum bit) {
		return (this = this | bit);
	}

	C_BitField operator| (const C_BitField bit) {
		C_BitField ret(*this);
		ret.SetFlags(bit);
		return ret;
	}

	C_BitField& operator&=(const Enum bit) {
		return (this = this & bit);
	}

	bool operator&(const Enum bit) const {
		return CheckFlag(bit);
	}

	value_type GetFlags() const {
		return m_Flags;
	}

protected:
	value_type m_Flags;
};
}