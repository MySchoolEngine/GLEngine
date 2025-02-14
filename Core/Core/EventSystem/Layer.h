#pragma once

#include <Core/CoreApi.h>
#include <Core/EventSystem/Event.h>
#include <Core/EventSystem/EventReciever.h>

namespace GLEngine::Core {

class CORE_API_EXPORT C_Layer : public I_EventReceiver {
public:
	explicit C_Layer(const std::string& name);
	~C_Layer() override = default;


	virtual void			   OnAttach() {}
	virtual void			   OnDetach() {}
	virtual void			   OnUpdate() {}
	void			   OnEvent(Core::I_Event& event) override {}
	[[nodiscard]] virtual bool ReadyForDestroy() const { return true; }

	[[nodiscard]] inline const std::string& GetName() const { return m_DebugName; }

protected:
	void SetLayerDebugName(const std::string& name);

private:
	std::string m_DebugName;
};

} // namespace GLEngine::Core