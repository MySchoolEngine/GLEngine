#pragma once

#include <glm/glm.hpp>

namespace GLEngine::Physics {
// atomic part of physics constraints
struct S_Atom {
public:
protected:
	glm::vec3 m_Position;
	float	  m_Weight;	// in kg
	bool	  m_bStatic; // cannot be moved
};
} // namespace GLEngine::Physics