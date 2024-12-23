#pragma once

#include <GUI/GUIApi.h>

namespace GLEngine::GUI {

class GUI_API_EXPORT I_GUIPart {
public:
	virtual ~I_GUIPart();
	/**
	 *
	 * @return true if menu item have been called
	 */
	[[nodiscard]] virtual bool Draw() const = 0;
};

} // namespace GLEngine::GUI