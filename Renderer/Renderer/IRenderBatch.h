#pragma once

namespace GLEngine { namespace Renderer {

//================================================
// List of I_RenderCommands
// should be in this order:
// 1] state changing commands
// 2] draw calls
// 3] possible getters
// would be nice to be able to define dependencies between batches to e.g. push all
// shadow map draw calls at beginning, than calculate smth else and draw calls which need to
// read from shadow map at the end
//================================================
class I_RenderBatch {
public:
	virtual ~I_RenderBatch() = default;
};

}} // namespace GLEngine::Renderer