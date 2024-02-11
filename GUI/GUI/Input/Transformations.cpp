#include <GUIStdafx.h>

#include <GUI/Input/Transformations.h>

#include <Utils/Reflection/Metadata.h>

#include <GUI/ReflectionGUI.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <Utils/Serialization/GLMRTTI.h>

#include <rttr/registration>
#include <rttr/type>

RTTR_REGISTRATION
{
	using namespace ::Utils::Reflection;
	using namespace GLEngine::GUI::Input;
	rttr::registration::class_<C_Transformations>("Transformations")
		.constructor<>()(rttr::policy::ctor::as_object)
		.property("Translation", &C_Transformations::m_Translation)
			(
				rttr::policy::prop::bind_as_ptr, 
				RegisterMetaclass<MetaGUI::Vec3>(), 
				RegisterMetamember<UI::Vec3::Name>("Translate")
			)
		.property("Rotation", &C_Transformations::m_Rotation)
			(
				rttr::policy::prop::bind_as_ptr, 
				RegisterMetaclass<MetaGUI::Vec3>(), 
				RegisterMetamember<UI::Vec3::Name>("Rotation"),
				RegisterMetamember<SerializationCls::NoSerialize>(true)
			)
		.property("Scale", &C_Transformations::m_Scale)
			(
				rttr::policy::prop::bind_as_ptr, 
				RegisterMetaclass<MetaGUI::Vec3>(), 
				RegisterMetamember<UI::Vec3::Name>("Scale")
			)
		.property("RotationDeg", &C_Transformations::GetRotationDeg, &C_Transformations::SetRotationDeg);
}

namespace GLEngine::GUI::Input {


//=================================================================================
C_Transformations::C_Transformations(glm::mat4 transformation, ::Utils::C_BitField<E_Transorms> enableTransforms)
	: m_enabledTransforms(enableTransforms)
	, m_Translation(glm::vec3(0.f))
	, m_Rotation(glm::vec3(0.f))
	, m_Scale(glm::vec3(1.f, 1.f, 1.f))
{
	SetMatrix(transformation);
}

//=================================================================================
C_Transformations::C_Transformations()
	: m_enabledTransforms({E_Transorms::Translate, E_Transorms::Rotate, E_Transorms::Scale})
	, m_Translation(glm::vec3(0.f))
	, m_Rotation(glm::vec3(0.f))
	, m_Scale(glm::vec3(1.f, 1.f, 1.f))
{
}

//=================================================================================
bool C_Transformations::Draw() const
{
	bool		   changed = false;
	rttr::instance obj(*this);
	if (m_enabledTransforms.CheckFlag(E_Transorms::Translate))
	{
		changed |= GUI::DrawPropertyGUI(obj, rttr::type::get<C_Transformations>().get_property("Translation"));
	}
	if (m_enabledTransforms.CheckFlag(E_Transorms::Rotate))
	{
		changed |= GUI::DrawPropertyGUI(obj, rttr::type::get<C_Transformations>().get_property("Rotation"));
	}
	if (m_enabledTransforms.CheckFlag(E_Transorms::Scale))
	{
		changed |= GUI::DrawPropertyGUI(obj, rttr::type::get<C_Transformations>().get_property("Scale"));
	}
	return changed;
}

//=================================================================================
glm::mat4 C_Transformations::GetMatrix() const
{
	glm::mat4 transform(1.f);
	if (m_enabledTransforms.CheckFlag(E_Transorms::Translate))
	{
		transform = glm::translate(transform, m_Translation);
	}
	if (m_enabledTransforms.CheckFlag(E_Transorms::Rotate))
	{
		transform = glm::rotate(transform, m_Rotation.x, glm::vec3(1, 0, 0));
		transform = glm::rotate(transform, m_Rotation.y, glm::vec3(0, 1, 0));
		transform = glm::rotate(transform, m_Rotation.z, glm::vec3(0, 0, 1));
	}
	if (m_enabledTransforms.CheckFlag(E_Transorms::Scale))
	{
		transform = glm::scale(transform, m_Scale);
	}
	return transform;
}

//=================================================================================
const glm::vec3& C_Transformations::GetTranslation() const
{
	return m_Translation;
}

//=================================================================================
const glm::vec3& C_Transformations::GetRotation() const
{
	return m_Rotation;
}

//=================================================================================
const glm::vec3& C_Transformations::GetScale() const
{
	return m_Scale;
}

//=================================================================================
void C_Transformations::SetMatrix(const glm::mat4& mat)
{
	glm::vec3 scale;
	glm::quat rotation;
	glm::vec3 translation;
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(mat, scale, rotation, translation, skew, perspective);
	m_Scale		  = scale;
	m_Translation = translation;
	m_Rotation	  = glm::degrees(glm::eulerAngles(glm::conjugate(rotation)));
}

//=================================================================================
void C_Transformations::SetEnabledTransforms(::Utils::C_BitField<E_Transorms> enableTransforms)
{
	m_enabledTransforms = enableTransforms;
}

//=================================================================================
void C_Transformations::SetTranslation(const glm::vec3& translation)
{
	m_Translation = translation;
}

//=================================================================================
void C_Transformations::SetRotation(const glm::vec3& rotation)
{
	m_Rotation = rotation;
}

//=================================================================================
void C_Transformations::SetScale(const glm::vec3& scale)
{
	m_Scale = scale;
}

//=================================================================================
glm::vec3 C_Transformations::GetRotationDeg() const
{
	return glm::degrees(m_Rotation);
}

//=================================================================================
void C_Transformations::SetRotationDeg(glm::vec3 rotation)
{
	m_Rotation = glm::radians(rotation);
}

} // namespace GLEngine::GUI::Input
