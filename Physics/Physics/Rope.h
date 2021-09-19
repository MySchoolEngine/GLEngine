#pragma once

#include <Physics/Atom.h>
#include <Physics/Spring.h>

#include <Renderer/PhysicsDebugDraw.h>

namespace GLEngine::Physics {

// Rope is straight line of atoms and springs
// Needs to start with atom, and end with atom
// Preserves uniform weight and stiffness across whole rope
// Can have first or last atom static and different weight (simulates weight connected to the end of rope)
struct C_Rope {
public:
	C_Rope();
	void Update();

	void DebugDraw(Renderer::PhysicsDebugDraw& dd) const;

	void AddSpring();
	//! Adds atom into the rope
	int AddAtom();
protected:
	// static data
	std::vector<S_Atom>	  m_Atoms;
	std::vector<S_Spring> m_Springs;
};
} // namespace GLEngine::Physics