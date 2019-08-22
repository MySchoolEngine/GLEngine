#pragma once

#include <Utils/Allocation/Internal/Traits.h>

#include <memory>

namespace Utils::Allocation {

template<typename T, class Primary, class Secondary>
class C_FallbackAllocator
	: private Primary
	, private Secondary
{
public:
	using size_type		= std::size_t;
	using pointer		= T*;
	using const_pointer = const T*;

	pointer allocate(size_type n, const void *hint = 0)
	{
		auto* ptr = Primary::allocate(n, hint);
		if (!ptr)
		{
			return Secondary::allocate(n, hint);
		}
		return ptr;
	}

	void deallocate(pointer p, size_type n)
	{
		if (Primary::owns(p))
		{
			return Primary::deallocate(p, n);
		}
		return Secondary::deallocate(p, n);
	}

	bool owns(pointer p)
	{
		return Primary::owns(p) || Secondary::owns(p);
	}

	typename std::enable_if<
		Traits::has_deallocate_all<Primary>::value &&
		Traits::has_deallocate_all<Secondary>::value, void>::type
		deallocateAll()
	{
		Primary::deallocateAll();
		Secondary::deallocateAll();
	}
};

}