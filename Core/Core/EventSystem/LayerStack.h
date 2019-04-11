#pragma once

#include <Core/EventSystem/Layer.h>

namespace GLEngine {
namespace Core {
class API_EXPORT C_LayerStack : public C_Layer {
public:
	C_LayerStack(std::string& name);
	virtual ~C_LayerStack();

	virtual void PushLayer(std::shared_ptr<C_Layer> layer);

	//===================================================
	// C_Layer
	//===================================================
	virtual void OnEvent(Core::I_Event& event) override;
protected:
	std::vector<std::shared_ptr<C_Layer>>* m_Layers;
};

}}