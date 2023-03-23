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

enum class E_RenderPrimitives
{
	TriangleList,
	LineList,
	PointList
};
} // namespace GLEngine::Renderer
