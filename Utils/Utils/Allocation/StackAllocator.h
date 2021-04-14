#pragma once

#include <Utils/Allocation/Internal/AllocatorUtils.h>

#include <memory>

namespace Utils::Allocation {

template <std::size_t size, std::size_t alignment = 4> class C_StackAllocator {
public:
	using size_type = std::size_t;

	C_StackAllocator()
		: m_head(m_memory)
	{
	}

	constexpr static size_type capacity() { return size; }

	constexpr size_type max_size() const { return size; }

	/************************************************************************/
	/* Allocates n bytes of memory.                                         */
	/************************************************************************/
	void* allocate(size_type n)
	{
		const auto n1 = AllocUtils::roundToAligned(n);
		if (n1 > static_cast<size_type>((m_memory + size) - m_head))
		{
			return nullptr;
		}
		auto result = reinterpret_cast<void*>(m_head);
		m_head += n1;
		return result;
	}

	template <class T> T* allocate() { return static_cast<T*>(allocate(sizeof(T))); }

	template <class T> T* allocateArray(size_type n) { return static_cast<T*>(allocate(sizeof(T) * n)); }

	template <class T> void deallocate(T* p)
	{
		if (reinterpret_cast<decltype(m_head)>(p) + AllocUtils::roundToAligned(sizeof(T)) == m_head)
		{
			m_head = reinterpret_cast<decltype(m_head)>(p);
		}
	}

	template <class T> void deallocateArray(T* p, size_type n)
	{
		if (reinterpret_cast<decltype(m_head)>(p) + AllocUtils::roundToAligned(sizeof(T) * n) == m_head)
		{
			m_head = reinterpret_cast<decltype(m_head)>(p);
		}
	}

	void deallocateAll() { m_head = m_memory; }

	template <class T> bool owns(T* p) { return static_cast<decltype(m_head)>(p) >= m_memory && static_cast<decltype(m_head)>(p) < m_memory + size; }

private:
	char  m_memory[size];
	char* m_head;

	using AllocUtils = C_AllocatorUtils<C_StackAllocator<size, alignment>, alignment>;
};

} // namespace Utils::Allocation