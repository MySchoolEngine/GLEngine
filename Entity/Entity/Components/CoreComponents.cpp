#include <EntityStdafx.h>

#include <Entity/Components/CoreComponents.h>

#include <Utils/Serialization/SerializationUtils.h>

#include <rttr/registration>

RTTR_REGISTRATION
{
	using namespace GLEngine::Entity;
	rttr::registration::class_<S_TransformComponent>("TransformComponent")
		.constructor<>()(rttr::policy::ctor::as_object)
		.property("Translation", &S_TransformComponent::translation)
		.property("Rotation", &S_TransformComponent::rotation)(REGISTER_DEFAULT_VALUE(glm::quat(1.f, 0.f, 0.f, 0.f)))
		.property("Scale", &S_TransformComponent::scale)(REGISTER_DEFAULT_VALUE(glm::vec3(1.f, 1.f, 1.f)));
}
