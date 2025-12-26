#pragma once

#pragma warning(push)
#pragma warning(disable : 4996)
#ifdef __GNUC__
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
#include <slot_map/slot_map.h>
#ifdef __GNUC__
	#pragma GCC diagnostic pop
#endif
#pragma warning(pop) 

namespace GLEngine::Renderer {
template <class Badge> using resource_index_t = dod::slot_map_key32<Badge>;

struct Buffer;	 // < does not need to be implemented, just badge
struct Shader;	 // < does not need to be implemented, just badge
struct Pipeline; // < does not need to be implemented, just badge
struct Texture;	 // < does not need to be implemented, just badge
struct Sampler;	 // < does not need to be implemented, just badge

template <class Badge> class Handle final {
public:
	using index_t					= resource_index_t<Badge>;
	static inline index_t BAD_INDEX = resource_index_t<Badge>::invalid();
	Handle()
		: m_Index(BAD_INDEX)
	{
	}
	[[nodiscard]] bool IsValid() const { return m_Index != BAD_INDEX; }

	[[nodiscard]] bool operator==(const Handle& other) const { return m_Index == other.m_Index; };
	[[nodiscard]] bool operator!=(const Handle& other) const { return m_Index != other.m_Index; };

	explicit operator bool() const { return IsValid(); }

private:
	explicit Handle(index_t index)
		: m_Index(index)
	{
	}

	index_t m_Index = BAD_INDEX;

	template <typename T, typename U> friend class ResourcePool;
};
} // namespace GLEngine::Renderer