#include <GLRendererStdafx.h>

#include <GLRenderer/GUI/LambdaPart.h>


namespace GLEngine::GLRenderer::GUI {

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

}