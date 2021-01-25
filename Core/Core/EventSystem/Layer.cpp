#include <CoreStdafx.h>

#include <Core/EventSystem/Layer.h>

namespace GLEngine { namespace Core {

//=================================================================================
C_Layer::C_Layer(const std::string& name)
	: m_DebugName(name)
{
}

//=================================================================================
void C_Layer::SetLayerDebugName(const std::string& name)
{
	m_DebugName = name;
}

}} // namespace GLEngine::Core