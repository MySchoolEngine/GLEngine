#pragma once

/**
 * This file contains all cross renderer definitions.
 */

namespace GLEngine::Renderer {
enum class E_ShaderStage
{
	Vertex,
	Fragment,
	Geometry,
	TesselationControl,
	TesselationEvaluation,
	Compute,
	RayGen,
};
}

