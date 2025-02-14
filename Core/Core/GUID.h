#pragma once

#include <Core/CoreApi.h>

#include <rttr/type>
#include <crossguid/guid.hpp>

namespace GLEngine {

// wrapper to allow change of guid implementation
// and avoid linking crossguid to all modules
class CORE_API_EXPORT GUID {
public:
	explicit GUID(std::string_view fromString);
	GUID(xg::Guid);
	GUID(); // constructs invalid string

	const static GUID INVALID_GUID;


	GUID(const GUID& other) = default;
	GUID& operator=(const GUID& other) = default;
	GUID(GUID&& other)				   = default;
	GUID& operator=(GUID&& other) = default;

	[[nodiscard]] bool operator==(const GUID& other) const;
	[[nodiscard]] bool operator!=(const GUID& other) const;

	[[nodiscard]] bool isValid() const;

	[[nodiscard]] std::string toString() const;
	[[nodiscard]]			  operator std::string() const;

	void swap(GUID& guid) noexcept;

	[[nodiscard]] std::size_t hash() const;

private:
	xg::Guid m_Guid;

	CORE_API_EXPORT friend bool operator<(const GUID& lhs, const GUID& rhs);
};

CORE_API_EXPORT GUID NextGUID();

} // namespace GLEngine

// need this for std containers
namespace std {
template <> void swap(GLEngine::GUID& guid0, GLEngine::GUID& guid1) noexcept;

template <> struct hash<GLEngine::GUID> {
	std::size_t operator()(GLEngine::GUID const& guid) const { return guid.hash(); }
};
} // namespace std