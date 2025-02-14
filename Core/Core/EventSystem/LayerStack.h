#pragma once

#include <Core/EventSystem/Layer.h>

namespace GLEngine::Core {
class CORE_API_EXPORT C_LayerStack : public C_Layer {
public:
	explicit C_LayerStack(std::string&& name);
	~C_LayerStack() override;

	virtual void PushLayer(C_Layer* layer);

	//===================================================
	// C_Layer
	//===================================================
	void			   OnUpdate() override;
	void			   OnEvent(I_Event& event) override;
	[[nodiscard]] bool ReadyForDestroy() const override;

protected:
	std::vector<C_Layer*> m_Layers;
};

} // namespace GLEngine::Core