#include <GUIStdafx.h>

#include <GUI/LambdaPart.h>


namespace GLEngine::GUI {

//=================================================================================
C_LambdaPart::C_LambdaPart(const std::function<bool()>& lambda)
	: m_Lambda(lambda)
{
}

//=================================================================================
bool C_LambdaPart::Draw() const
{
	return m_Lambda();
}

} // namespace GLEngine::GUI