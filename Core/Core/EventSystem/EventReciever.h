#pragma once

#include <Core/CoreApi.h>

namespace GLEngine::Core {
class I_Event;

class CORE_API_EXPORT I_EventReceiver {
public:
	virtual ~I_EventReceiver();
	virtual void OnEvent(I_Event& event) = 0;
};

} // namespace GLEngine::Core