#pragma once


namespace GLEngine {
namespace GLRenderer {
namespace GUI {

class I_GUIComponent {
public:
	virtual ~I_GUIComponent() = default;
	virtual void Draw() const = 0;
};

}}}