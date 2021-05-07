#pragma once

namespace GLEngine::Utils {

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

template <typename T, typename S> void removeIndicesFromVector(std::vector<T>& v, const S& rm)
{
	GLE_ASSERT(std::is_sorted(std::begin(rm), std::end(rm)), "Unsorted indices to delete");
	return removeIndicesFromVector(v, std::begin(rm), std::end(rm));
}

template<class T, class S>
[[nodiscard]] bool contains(const std::set<T>& v, const S& val)
{
	if (v.find(val) != v.end())
		return true;
	return false;
}


} // namespace GLEngine::Utils
