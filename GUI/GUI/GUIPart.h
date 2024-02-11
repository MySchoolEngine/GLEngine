#pragma once

#include <GUI/GUIApi.h>

namespace GLEngine::GUI {

class GUI_API_EXPORT I_GUIPart {
public:
	virtual ~I_GUIPart();
	virtual bool Draw() const = 0;
};

} // namespace GLEngine::GUI