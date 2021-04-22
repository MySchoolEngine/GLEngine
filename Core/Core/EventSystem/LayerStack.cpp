#include <CoreStdafx.h>

#include <Core/EventSystem/LayerStack.h>


namespace GLEngine::Core {

//=================================================================================
C_LayerStack::C_LayerStack(std::string&& name)
	: C_Layer(std::move(name))
{
}

//=================================================================================
C_LayerStack::~C_LayerStack() = default;

//=================================================================================
void C_LayerStack::PushLayer(C_Layer* layer)
{
	m_Layers.push_back(layer);
}

//=================================================================================
void C_LayerStack::OnEvent(Core::I_Event& event)
{
	for (auto& layer : m_Layers)
	{
		layer->OnEvent(event);
		if (event.m_Handeld)
			break;
	}
}

//=================================================================================
bool C_LayerStack::ReadyForDestroy() const
{
	return std::all_of(m_Layers.begin(), m_Layers.end(), [](const auto& it) { return it->ReadyForDestroy(); });
}

} // namespace GLEngine::Core