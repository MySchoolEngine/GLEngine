#pragma once

#include <GUI/GUIPart.h>

#include <DULib/BitField.h>
#include <rttr/registration_friend.h>

// TODO: This class should not be in GUI module at all! This should go to the Entity??
namespace GLEngine::GUI::Input {
class GUI_API_EXPORT C_Transformations : public I_GUIPart {
public:
	enum class E_Transforms : std::uint8_t
	{
		Translate = 1,
		Rotate	  = 2,
		Scale	  = 4,
	};
	C_Transformations(const glm::mat4& transformation, DULib::BitField<E_Transforms> enableTransforms = {E_Transforms::Translate, E_Transforms::Rotate, E_Transforms::Scale});
	C_Transformations();

	void SetEnabledTransforms(DULib::BitField<E_Transforms> enableTransforms);
	//=================================================================================
	bool Draw() const override;

	[[nodiscard]] glm::mat4		   GetMatrix() const;
	void						   SetMatrix(const glm::mat4& mat);
	[[nodiscard]] const glm::vec3& GetTranslation() const;
	void						   SetTranslation(const glm::vec3& translation);
	[[nodiscard]] const glm::vec3& GetRotation() const;
	void						   SetRotation(const glm::vec3& rotation);
	[[nodiscard]] const glm::vec3& GetScale() const;
	void						   SetScale(const glm::vec3& scale);
	[[nodiscard]] glm::vec3		   GetRotationDeg() const;
	void						   SetRotationDeg(glm::vec3 rotation);

	bool operator==(const C_Transformations&) const;

private:
	DULib::BitField<E_Transforms> m_EnabledTransforms;
	glm::vec3					  m_Translation;
	glm::vec3					  m_Rotation; //< in rad
	glm::vec3					  m_Scale;

	RTTR_ENABLE()
	RTTR_REGISTRATION_FRIEND
};
} // namespace GLEngine::GUI::Input

//=================================================================================
template <> struct DULib::enable_BitField_operators<GLEngine::GUI::Input::C_Transformations::E_Transforms> {
	static constexpr bool enable = true;
};
template <> struct DULib::BitField_UsedBitsCounter<GLEngine::GUI::Input::C_Transformations::E_Transforms> {
	static constexpr std::size_t usedBits = 3;
};