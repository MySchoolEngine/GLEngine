#pragma once

#include <CoreStdafx.h>

#include <Core/EventSystem/Event.h>

namespace GLEngine {
namespace Core {
//=============================================================
// @todo: should also contain other events in the future
class C_AppEvent : public I_Event {
public:
	C_AppEvent()
	{}

	inline float GetPosX() const { return m_PosX; }
	inline float GetPosY() const { return m_PosY; }

	EVENT_CLASS_CATEGORY(E_EventCategory::None);
	EVENT_CLASS_TYPE(AppEvent);
private:
	float m_PosX;
	float m_PosY;
};

}}