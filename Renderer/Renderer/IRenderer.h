#pragma once

#include <Core/CoreMacros.h>

#include <Renderer/IRenderBatch.h>
#include <Renderer/IRenderCommand.h>

#include <memory>

namespace GLEngine {
namespace Renderer {

// We need some separated tasks -> game engine should give us list of all visible objects
// or some way to ask him what is inside given frustum
// than we need to be able to collect data to internal structures
// than convert them to GPU-friendly data
// than collect draw calls, render batches etc.
// sort them to some convenient order, and commit them to GPU
// in this way we can split one frame to separated parts
// 1] simulation
// 2] data collection (simulation have to be stopped in this moment, so everything we would be able to do here is just collection in tight loop)
// 3] data conversion - now we can begin with simulation of new frame
// 4] commit to GPU
// 5] check the time, if we have enough time, render thread can help with simulation, game computation etc.
// 6] render new frame
class API_EXPORT I_Renderer {
public:
	using T_CommandPtr = std::unique_ptr<I_RenderCommand>;
	using T_BatchPtr = std::unique_ptr<I_RenderBatch>;

	virtual ~I_Renderer() = default;
	virtual void AddCommand(T_CommandPtr) = 0;
	virtual void AddBatch(T_BatchPtr) = 0;

	/** ==============================================
	 * @method:    SortCommands
	 * @return:    void
	 * @brief	   Reorganize all commands for better performance
	 ** ==============================================*/
	virtual void SortCommands() = 0;

	/** ==============================================
	 * @method:    ExtractData
	 * @return:    void
	 * @brief	   By now we should have all commands to render the frame
	 *			   and we start with extraction of I_RawGPUData
	 ** ==============================================*/
	virtual void ExtractData() = 0;

	/** ==============================================
	 * @method:    TransformData
	 * @return:    void
	 * @brief	   After we collected all data, we have time to transform them
	 *			   in this time the simulation thread is free to change 
	 *			   it's renderable data
	 ** ==============================================*/
	virtual void TransformData() = 0;

	/** ==============================================
	* @method:    Commit
	* @return:    void
	* @brief	   Commits all GPU calls to GPU
	** ==============================================*/
	virtual void Commit() const = 0;

	/** ==============================================
	* @method:    ClearCommandBuffers
	* @return:    void
	* @brief	   Should be called at the end of frame.
	** ==============================================*/
	virtual void ClearCommandBuffers() = 0;
};

}
}