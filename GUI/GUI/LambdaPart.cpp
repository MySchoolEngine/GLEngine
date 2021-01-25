#include <GUIStdafx.h>

#include <GUI/LambdaPart.h>


namespace GLEngine::GUI {

//=================================================================================
C_LambdaPart::C_LambdaPart(std::function<void()> lambda)
	: m_Lambda(lambda)
{
}

//=================================================================================
void C_LambdaPart::Draw() const
{
	m_Lambda();
}

} // namespace GLEngine::GUI