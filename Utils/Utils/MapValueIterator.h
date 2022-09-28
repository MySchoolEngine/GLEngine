#pragma once

namespace Utils {

template <class T> class MapValueIterator {
public:
	using difference_type	= typename T::difference_type;
	using value_type		= typename T::value_type;
	using pointer			= typename T::pointer;
	using reference			= typename T::reference;
	using iterator_category = typename T::iterator_category;

	MapValueIterator(T&& iter)
		: m_iter(std::move(iter))
	{
	}

	~MapValueIterator() = default;

	MapValueIterator(MapValueIterator&& other) noexcept
		: m_iter(std::move(other.m_iter))
	{
	}

	MapValueIterator(const MapValueIterator& other)
		: m_iter(other.m_iter)
	{
	}

	[[nodiscard]] auto operator*() const { return m_iter->second; }
	MapValueIterator& operator++()
	{
		++m_iter;
		return *this;
	}
	[[nodiscard]] bool operator!=(const MapValueIterator& other) const { return m_iter != other.m_iter; }
	[[nodiscard]] bool operator==(const MapValueIterator& other) const { return m_iter == other.m_iter; }

private:
	T m_iter;
};
} // namespace Utils
