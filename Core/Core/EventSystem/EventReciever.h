#pragma once

#include <Core/CoreApi.h>

#include <Core/EventSystem/Event.h>

namespace GLEngine::Core {

class CORE_API_EXPORT I_EventReciever {
public:
	virtual ~I_EventReciever();
	virtual void OnEvent(Core::I_Event& event) = 0;
};

} // namespace GLEngine::Core