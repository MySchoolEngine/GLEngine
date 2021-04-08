#pragma once

#include <atomic>

namespace Utils {

struct SingleThreadCounter {
	using T_CounterType = std::size_t;
};

struct MultiThreadCounter {
	using T_CounterType = std::atomic<std::size_t>;
};

template <class ThreadPolicy> class RefCounted {
public:
	constexpr RefCounted()
		: m_UseCount(0)
	{
	}
	constexpr void		  AddRef() { m_UseCount++; }
	constexpr void		  RemCount() { m_UseCount--; }
	constexpr std::size_t UseCount() const { return m_UseCount; }
	constexpr bool		  IsReferenced() const { return m_UseCount != 0; }

private:
	typename ThreadPolicy::T_CounterType m_UseCount;
};
} // namespace Utils
