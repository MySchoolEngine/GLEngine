#include <CoreStdafx.h>

#include <Core/EventSystem/LayerStack.h>


namespace GLEngine {
namespace Core {

//=================================================================================
C_LayerStack::C_LayerStack(std::string& name)
	: C_Layer(name)
	, m_Layers(new std::remove_pointer<decltype(m_Layers)>::type)
{}

//=================================================================================
C_LayerStack::~C_LayerStack()
{
	delete m_Layers;
}

//=================================================================================
void C_LayerStack::PushLayer(std::shared_ptr<C_Layer> layer)
{
	m_Layers->push_back(layer);
}

//=================================================================================
void C_LayerStack::OnEvent(Core::I_Event& event)
{
	for (auto& layer : *m_Layers)
	{
		layer->OnEvent(event);
		if (event.m_Handeld) break;
	}
}

}}