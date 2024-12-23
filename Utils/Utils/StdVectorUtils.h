#pragma once

#include <Utils/Logging/LoggingMacros.h>
#include <Utils/Logging/LoggingTypes.h>

#include <algorithm>
#include <set>
#include <vector>

namespace Utils {

// Source:
// https://codereview.stackexchange.com/questions/206686/removing-by-indices-several-elements-from-a-vector
template <typename T, typename Iter> void removeIndicesFromVector(std::vector<T>& v, Iter begin, Iter end)
{
	GLE_ASSERT(std::is_sorted(begin, end), "Unsorted indices to delete");
	auto		rm_iter		  = begin;
	std::size_t current_index = 0;

	const auto pred = [&](const T&) {
		// any more to remove?
		if (rm_iter == end)
		{
			return false;
		}
		// is this one specified?
		if (*rm_iter == current_index++)
		{
			return ++rm_iter, true;
		}
		return false;
	};

	v.erase(std::remove_if(v.begin(), v.end(), pred), v.end());
}

//=================================================================================
template <typename Container> 
auto min_element(const Container& c)
{
	return std::min_element(c.cbegin(), c.cend());
}

//=================================================================================
template <typename Container>
auto max_element(const Container& c)
{
	return std::max_element(c.cbegin(), c.cend());
}

//=================================================================================
template <typename T, typename S> void removeIndicesFromVector(std::vector<T>& v, const S& rm)
{
	GLE_ASSERT(std::is_sorted(std::begin(rm), std::end(rm)), "Unsorted indices to delete");
	return removeIndicesFromVector(v, std::begin(rm), std::end(rm));
}

//=================================================================================
template <class T, class S> [[nodiscard]] bool contains(const std::set<T>& v, const S& val)
{
	if (v.contains(val))
		return true;
	return false;
}

//=================================================================================
template <class T, class S> [[nodiscard]] bool contains(const std::vector<T>& v, const S& val)
{
	if (std::find(v.begin(), v.end(), val) != v.end())
		return true;
	return false;
}

//=================================================================================
// Deletes all indexes defined in set from vector. Leverages the advantage of set being
// sorted from the smallest to the highest number and continuity of vector storage.
template <class T, class IdxT> typename std::vector<T>::iterator remove_indices(std::vector<T>& vector, const std::set<IdxT>& set)
{
	if (set.empty())
	{
		return vector.end();
	}

	auto	   nextDelete		= set.begin();
	const auto destinationIndex = *nextDelete;
	auto	   destination		= vector.begin();
	auto	   source			= vector.begin();
	std::advance(destination, destinationIndex);
	std::advance(source, destinationIndex + 1);
	++nextDelete;
	auto sourceIndex = destinationIndex + 1;
	while (destination != vector.end() - set.size())
	{
		while (nextDelete != set.end())
		{
			if (sourceIndex == *nextDelete)
			{
				++source;
				++nextDelete;
				++sourceIndex;
			}
		}
		*destination = std::move(*source);
		++destination;
		++source;
		++sourceIndex;
	}
	return destination;
}

// result = {result, append}
template <class T> void appendVector(std::vector<T>& result, const std::vector<T>& append)
{
	result.insert(std::end(result), std::begin(append), std::end(append));
}

// returns {a, b}
template <class T> std::vector<T> mergeVector(const std::vector<T>& a, const std::vector<T>& b)
{
	std::vector<T> result = a;
	appendVector(result, b);
	return result;
}

} // namespace Utils
