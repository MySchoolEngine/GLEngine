#include <GLRendererStdafx.h>

#include <GLRenderer/GLFW/GLFWInput.h>

#include <GLRenderer/GLFW/GLFWWindow.h>

namespace GLEngine {
namespace GLRenderer {
namespace GLFW {

//=================================================================================
C_GLFWInput::C_GLFWInput()
	: m_Window(nullptr)
{

}

//=================================================================================
C_GLFWInput::~C_GLFWInput() = default;

//=================================================================================
bool C_GLFWInput::IsKeyPressed(int keycode) const
{
	auto state = glfwGetKey(m_Window, keycode);
	return state == GLFW_PRESS || state == GLFW_REPEAT;
}

//=================================================================================
bool C_GLFWInput::IsMosueButtonPresse(int key) const
{
	auto state = glfwGetMouseButton(m_Window, key);
	return state == GLFW_PRESS;
}

//=================================================================================
std::pair<float, float> C_GLFWInput::GetMousePosition() const
{
	std::pair<double, double> pos;
	glfwGetCursorPos(m_Window, &pos.first, &pos.second);
	std::pair<float, float> floatPos(static_cast<float>(pos.first), static_cast<float>(pos.second));

	return floatPos;
}

//=================================================================================
float C_GLFWInput::GetMouseX() const
{
	return GetMousePosition().first;
}

//=================================================================================
float C_GLFWInput::GetMouseY() const
{
	return GetMousePosition().second;
}

}}}