#include <GUIStdafx.h>

#include <GUI/Input/Vector.h>

namespace GLEngine::GUI::Input {

//=================================================================================
C_Vec3::C_Vec3(std::string&& name, glm::vec3 val)
	: C_Vector(std::move(name), val)
{
}

//=================================================================================
bool C_Vec3::Draw() const
{
	float* col = &(m_Value.x);
	return ::ImGui::InputFloat3(m_Name.c_str(), col);
}

//=================================================================================
C_Vec3& C_Vec3::operator=(const glm::vec3& value)
{
	m_Value = value;
	return *this;
}

} // namespace GLEngine::GUI::Input