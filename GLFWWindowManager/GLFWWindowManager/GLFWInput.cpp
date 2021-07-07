#include <GLFWWindowManagerStdafx.h>

#include <GLFWWindowManager/GLFWInput.h>
#include <GLFWWindowManager/GLFWWindow.h>

namespace GLEngine::GLFWManager {

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

//=================================================================================
glm::vec2 C_GLFWInput::GetClipSpaceMouseCoord() const
{
	const auto screenCoord = GetMousePosition();
	glm::ivec2 windowSize;
	glfwGetWindowSize(m_Window, &(windowSize.x), &(windowSize.y));
	const float x = (2.0f * screenCoord.first) / windowSize.x - 1.0f;
	const float y = 1.0f - (2.0f * screenCoord.second) / windowSize.y;
	return {x, y};
}

//=================================================================================
Utils::C_BitField<Core::E_KeyModifiers> TranslateGLFWModifiers(int mods)
{
	return static_cast<Core::E_KeyModifiers>(mods);
}

} // namespace GLEngine::GLFWManager