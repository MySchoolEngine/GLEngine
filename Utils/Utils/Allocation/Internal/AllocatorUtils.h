#pragma once


namespace Utils::Allocation {
template <class T, std::size_t alignment> class C_AllocatorUtils {
public:
	//=================================================================================
	static typename T::size_type roundToAligned(typename T::size_type n)
	{
		auto floor = (n / alignment) * alignment;

		if (floor == n)
		{
			return floor;
		}
		else
		{
			return floor + alignment;
		}
	}
};

} // namespace Utils::Allocation