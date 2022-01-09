#pragma once

#include <GUI/GUIPart.h>
#include <GUI/Input/Vector.h>

#include <Utils/BitField.h>

#include <rttr/registration_friend.h>

// TODO: This class should not be in GUI module at all! This should go to the Entity??
namespace GLEngine::GUI::Input {
class GUI_API_EXPORT C_Transformations : public I_GUIPart {
public:
	enum class E_Transorms
	{
		Translate = 1,
		Rotate	  = 2,
		Scale	  = 4,
	};
	C_Transformations(glm::mat4 transformation, Utils::C_BitField<E_Transorms> enableTransforms = {E_Transorms::Translate, E_Transorms::Rotate, E_Transorms::Scale});
	C_Transformations();

	void SetEnabledTransforms(Utils::C_BitField<E_Transorms> enableTransforms);
	//=================================================================================
	void Draw() const override;

	[[nodiscard]] glm::mat4		   GetMatrix() const;
	void						   SetMatrix(const glm::mat4& mat);
	[[nodiscard]] const glm::vec3& GetTranslation() const;
	void						   SetTranslation(const glm::vec3& translation);
	[[nodiscard]] const glm::vec3& GetRotation() const;
	void						   SetRotation(const glm::vec3& rotation);
	[[nodiscard]] const glm::vec3& GetScale() const;
	void						   SetScale(const glm::vec3& scale);

private:
	Utils::C_BitField<E_Transorms> m_enabledTransforms;
	glm::vec3					   m_Translation;
	glm::vec3					   m_Rotation;
	glm::vec3					   m_Scale;

	RTTR_ENABLE();
	RTTR_REGISTRATION_FRIEND;
};
} // namespace GLEngine::GUI::Input

//=================================================================================
template <> struct Utils::enable_BitField_operators<GLEngine::GUI::Input::C_Transformations::E_Transorms> {
	static constexpr bool enable = true;
};