#pragma once

#include <slot_map/slot_map.h>

namespace GLEngine::Renderer {
template <class Badge> using resource_index_t = dod::slot_map_key32<Badge>;

struct Buffer;	// < does not need to be implemented, just badge
struct Shader;	// < does not need to be implemented, just badge
struct Texture; // < does not need to be implemented, just badge
struct Sampler; // < does not need to be implemented, just badge

template <class Badge> class Handle final {
public:
	using index_t					= resource_index_t<Badge>;
	static inline index_t BAD_INDEX = resource_index_t<Badge>::invalid();
	Handle()
		: m_index(BAD_INDEX)
	{
	}
	[[nodiscard]] bool IsValid() const { return m_index != BAD_INDEX; }

	[[nodiscard]] bool operator==(const Handle& other) const { return m_index == other.m_index; };
	[[nodiscard]] bool operator!=(const Handle& other) const { return m_index != other.m_index; };

private:
	explicit Handle(index_t index)
		: m_index(index)
	{
	}

	index_t m_index = BAD_INDEX;

	template <typename T, typename U> friend class ResourcePool;
};
} // namespace GLEngine::Renderer