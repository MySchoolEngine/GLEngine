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
void C_LayerStack::OnEvent(I_Event& event)
{
	for (auto& layer : m_Layers)
	{
		if (event.m_Handled)
			break;
		layer->OnEvent(event);
	}
}

//=================================================================================
bool C_LayerStack::ReadyForDestroy() const
{
	return std::all_of(m_Layers.begin(), m_Layers.end(), [](const auto& it) { return it->ReadyForDestroy(); });
}

//=================================================================================
void C_LayerStack::OnUpdate()
{
	std::for_each(m_Layers.begin(), m_Layers.end(), [](auto& layer) { layer->OnUpdate(); });
}

} // namespace GLEngine::Core