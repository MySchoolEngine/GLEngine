#pragma once

#include <Core/GUID.h>

#include <entt/entt.hpp>
#include <string>

namespace GLEngine::Entity {

struct S_IdentityComponent {
	GUID		id;
	std::string name;
};

} // namespace GLEngine::Entity
