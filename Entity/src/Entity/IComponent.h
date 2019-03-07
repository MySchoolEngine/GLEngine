#pragma once


namespace GLEngine {
namespace Entity {

enum class E_ComponentType;

class I_Component {
public:
	virtual ~I_Component() = default;
	virtual E_ComponentType GetType() const = 0;
};

}
}