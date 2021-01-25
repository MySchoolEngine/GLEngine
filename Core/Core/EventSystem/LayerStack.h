#pragma once

#include <Core/EventSystem/Layer.h>

namespace GLEngine::Core {
class CORE_API_EXPORT C_LayerStack : public C_Layer {
public:
	explicit C_LayerStack(std::string&& name);
	virtual ~C_LayerStack();

	virtual void PushLayer(C_Layer* layer);

	//===================================================
	// C_Layer
	//===================================================
	virtual void OnEvent(Core::I_Event& event) override;

protected:
	std::vector<C_Layer*>* m_Layers;
};

} // namespace GLEngine::Core