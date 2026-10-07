#pragma once

#include <Core/GUID.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <cstdint>
#include <entt/entt.hpp>
#include <rttr/registration_friend.h>
#include <rttr/type>
#include <string>

namespace GLEngine::Entity {

struct S_IdentityComponent {
	GUID		id;
	std::string name;
};

struct S_TransformComponent {
	glm::vec3 translation{0.f, 0.f, 0.f};
	glm::quat rotation{1.f, 0.f, 0.f, 0.f};
	glm::vec3 scale{1.f, 1.f, 1.f};

	RTTR_REGISTRATION_FRIEND
};

struct S_WorldTransformComponent {
	glm::mat4 world{1.f};
};

struct S_RelationshipComponent {
	entt::entity  parent	 = entt::null;
	entt::entity  firstChild = entt::null;
	entt::entity  prev		 = entt::null;
	entt::entity  next		 = entt::null;
	std::uint32_t childCount = 0;
};

struct S_DirtyTransformTag {};

} // namespace GLEngine::Entity
