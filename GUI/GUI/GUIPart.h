#pragma once

#include <GUI/GUIApi.h>

namespace GLEngine::GUI {

class I_GUIPart {
public:
	virtual ~I_GUIPart() = default;
	virtual void Draw() const = 0;
};

}