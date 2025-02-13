#pragma once

namespace Utils {
template <class T, int num = 1> struct Padding {
private:
	T m_Space[num] = {0};
};
} // namespace Utils
