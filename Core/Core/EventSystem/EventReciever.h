#pragma once

#include <Core/CoreApi.h>

namespace GLEngine::Core {
class I_Event;

class CORE_API_EXPORT I_EventReciever {
public:
	virtual ~I_EventReciever();
	virtual void OnEvent(I_Event& event) = 0;
};

} // namespace GLEngine::Core