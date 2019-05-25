#pragma once


namespace GLEngine {
namespace GLRenderer {
namespace GUI {

class I_GUIPart {
public:
	virtual ~I_GUIPart() = default;
	virtual void Draw() const = 0;
};

}}}