#pragma once

#include <Renderer/RendererApi.h>
#include <Renderer/Resources/RenderResourceHandle.h>

namespace GLEngine::Renderer {

// 1] Collection - needs to go through scene, collect renderables - not implemented yet left for optimization
//    - so far we are only collecting everything
// 2] Binning - bins by the handles, allows for instancing
// 3] Sorting - sort by distance from camera - not implemented yet left for optimization
// 4] Render view

// Also for main pass this should allow to incorporate occlusion culling etc.

// binning first happens by shader, later by geometry
struct RenderCall3D {
	glm::mat4					  ModelMatrix;
	int							  MaterialIndex; // oddly specific now, but I will think about it
	std::array<Handle<Buffer>, 5> Buffers;		 // let's go for up to 5 attributes right now
	std::string					  Shader;		 // need to be a handle
};

class RenderInterface {
public:
	RenderInterface()		   = default;
	virtual ~RenderInterface() = default;

	// I need to draw, draw instanced etc.
	virtual void Render(const RenderCall3D& call) = 0;
};

// use this per view
class RENDERER_API_EXPORT Renderer3D {
public:
	// somewhere I need to get render camera
	void Draw(RenderCall3D&& call); // 1]
	void Clear();
	void Commit(RenderInterface& interface); // resource manager needed

private:
	std::vector<RenderCall3D> m_DrawCalls;
};
} // namespace GLEngine::Renderer
