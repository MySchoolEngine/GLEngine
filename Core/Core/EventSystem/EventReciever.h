#pragma once

#include <CoreStdafx.h>

#include <Core/EventSystem/Event.h>

namespace GLEngine {
namespace Core {

class CORE_API_EXPORT I_EventReciever {
public:
	virtual ~I_EventReciever() = default;
	virtual void OnEvent(Core::I_Event& event) = 0;
};

}
}