#pragma once

#include <CoreStdafx.h>

#include <Core/EventSystem/Event.h>

namespace GLEngine::Core {
//=============================================================
// @todo: should also contain other events in the future
class C_AppEvent : public I_Event {
public:
	C_AppEvent() = default;

	EVENT_CLASS_CATEGORY(E_EventCategory::None);
	EVENT_CLASS_TYPE(AppEvent);
};

//=============================================================
class C_WindowResizedEvent : public I_Event {
public:
	C_WindowResizedEvent(int width, int height)
		: m_Width(width)
		, m_Height(height)
	{
	}

	int GetWidth() const { return m_Width; }
	int GetHeight() const { return m_Height; }

	EVENT_CLASS_CATEGORY(E_EventCategory::Application);
	EVENT_CLASS_TYPE(WindowResized);

private:
	int m_Width;
	int m_Height;
};

} // namespace GLEngine::Core