#include <GUIStdAfx.h>

#include <GUI/Input/Transformations.h>

#include <glm/gtc/matrix_transform.hpp>

namespace GLEngine::GUI::Input {


//=================================================================================
C_Transformations::C_Transformations(glm::mat4& transformation, Utils::C_BitField<E_Transorms> enableTransforms)
	: m_enabledTransforms(enableTransforms)
	, m_Translation("Translate", glm::vec3(0.f))
	, m_Rotation("Rotation", glm::vec3(0.f))
	, m_Scale("Scale", glm::vec3(0.f))
{
}

//=================================================================================
void C_Transformations::Draw() const
{
	if (m_enabledTransforms.CheckFlag(E_Transorms::Translate))
	{
		m_Translation.Draw();
	}
	if (m_enabledTransforms.CheckFlag(E_Transorms::Rotate))
	{
		m_Rotation.Draw();
	}
	if (m_enabledTransforms.CheckFlag(E_Transorms::Scale))
	{
		m_Scale.Draw();
	}
}

//=================================================================================
glm::mat4 C_Transformations::GetMatrix() const
{
	glm::mat4 transform(1.f);
	if (m_enabledTransforms.CheckFlag(E_Transorms::Translate))
	{
		glm::translate(transform, GetTranslation());
	}
	if (m_enabledTransforms.CheckFlag(E_Transorms::Rotate))
	{
		const auto rotations = GetRotation();
		glm::rotate(transform, rotations.x, glm::vec3(1, 0, 0));
		glm::rotate(transform, rotations.y, glm::vec3(0, 1, 0));
		glm::rotate(transform, rotations.z, glm::vec3(0, 0, 1));
	}
	if (m_enabledTransforms.CheckFlag(E_Transorms::Scale))
	{
		glm::scale(transform, GetScale());
	}
}

//=================================================================================
glm::vec3 C_Transformations::GetTranslation() const
{
	m_Translation.GetValue();
}

//=================================================================================
glm::vec3 C_Transformations::GetRotation() const
{
	m_Rotation.GetValue();
}

//=================================================================================
glm::vec3 C_Transformations::GetScale() const
{
	m_Scale.GetValue();
}

} // namespace GLEngine::GUI::Input
