#pragma once

#include <memory>
#include <string>

namespace GLEngine {
namespace Entity {

class I_Component;

enum class E_ComponentType {
	Graphical,
};

class I_Entity {
public:
	virtual ~I_Entity() = default;
	virtual std::shared_ptr<I_Component> GetComponent(E_ComponentType type) const = 0;
	virtual std::string GetName() const = 0;
};

}
}