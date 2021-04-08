#pragma once

#include <Utils/Allocation/Internal/AllocatorUtils.h>

#include <memory>

namespace Utils::Allocation {

/************************************************************************/
/* This allocator is meant for std structures. It is not optimal for    */
/* usage in std::vector as it will not deallocate any memory during     */
/* its lifetime.                                                        */
/************************************************************************/
template <class T, std::size_t size, std::size_t alignment = 4> class C_StdStackAllocator {
public:
	using size_type		  = std::size_t;
	using pointer		  = T*;
	using const_pointer	  = const T*;
	using value_type	  = T;
	using difference_type = std::ptrdiff_t;

	using is_always_equal = std::false_type;

	C_StdStackAllocator()
		: m_head(m_memory)
		, m_Allocated(0)
	{
	}

	template <class U>
	C_StdStackAllocator(const C_StdStackAllocator<U, size, alignment>& other) noexcept
		: m_head(m_memory)
	{
	}

	template <class U> struct rebind {
		using other = C_StdStackAllocator<U, size, alignment>;
	};

	~C_StdStackAllocator() = default;

	constexpr static size_type capacity() { return size; }

	pointer allocate(size_type n, const void* hint = 0)
	{
		const auto n1 = AllocUtils::roundToAligned(n * sizeof(T));
		if (n1 > static_cast<size_type>((m_memory + size) - m_head))
		{
			return nullptr;
		}
		m_Allocated += n1;
		auto result = reinterpret_cast<pointer>(m_head);
		m_head += n1;
		return result;
	}

	void deallocate(pointer p, size_type n)
	{
		if (reinterpret_cast<decltype(m_head)>(p) + AllocUtils::roundToAligned(n * sizeof(T)) == m_head)
		{
			m_head = reinterpret_cast<decltype(m_head)>(p);
		}
	}

	size_type max_size() const { return size; }

	void deallocateAll() { m_head = m_memory; }

	bool owns(pointer p) { return p >= m_memory && p < m_memory + size; }

private:
	char  m_memory[size];
	char* m_head;

	size_type m_Allocated;

	using AllocUtils = C_AllocatorUtils<C_StdStackAllocator<T, size, alignment>, alignment>;
};

template <class T, std::size_t size, std::size_t alignment> bool operator==(const C_StdStackAllocator<T, size, alignment>& lhs, const C_StdStackAllocator<T, size, alignment>& rhs)
{
	return lhs.m_memory == rhs.m_memory;
}

template <class T, std::size_t size, std::size_t alignment> bool operator!=(const C_StdStackAllocator<T, size, alignment>& lhs, const C_StdStackAllocator<T, size, alignment>& rhs)
{
	return lhs.m_memory != rhs.m_memory;
}

} // namespace Utils::Allocation