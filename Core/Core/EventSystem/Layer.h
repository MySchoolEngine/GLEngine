#pragma once

#include <CoreStdafx.h>

#include <Core/EventSystem/Event.h>

#include <Utils/BitField.h>

namespace GLEngine {
namespace Core {

class API_EXPORT C_Layer {
public:
	C_Layer(const std::string& name);
	virtual ~C_Layer() = default;


	virtual void OnAttach() {}
	virtual void OnDetach() {}
	virtual void OnUpdate() {}
	virtual void OnEvent(Core::I_Event& event) {}

	inline const std::string& GetName() const { return m_DebugName; }
protected:
	void SetLayerDebugName(const std::string& name);
private:
#pragma warning(push)
#pragma warning( disable : 4251)
	std::string m_DebugName;
#pragma  warning(pop)
};

}}