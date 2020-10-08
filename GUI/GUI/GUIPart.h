#pragma once

#include <GUI/GUIApi.h>

namespace GLEngine::GUI {

class GUI_API_EXPORT I_GUIPart {
public:
	virtual ~I_GUIPart();
	virtual void Draw() const = 0;
};

}