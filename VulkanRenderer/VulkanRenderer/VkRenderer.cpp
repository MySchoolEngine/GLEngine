#include <VkRendererStdAfx.h>

#include <VulkanRenderer/VkRenderer.h>

#include <Renderer/IRenderBatch.h>
#include <Renderer/IRenderCommand.h>

namespace GLEngine::VkRenderer {

//=================================================================================
C_VkRenderer::C_VkRenderer()
	: m_CommandQueue(new std::remove_pointer<decltype(m_CommandQueue)>::type)
{

}

//=================================================================================
C_VkRenderer::~C_VkRenderer()
{
	delete m_CommandQueue;
}

//=================================================================================
void C_VkRenderer::Lock(bool lock /*= true*/)
{
	m_Locked = lock;
}

//=================================================================================
void C_VkRenderer::AddCommand(T_CommandPtr command)
{
	if (m_Locked) {
		__debugbreak();
	}
	m_CommandQueue->emplace_back(std::move(command));
}

//=================================================================================
void C_VkRenderer::AddBatch(T_BatchPtr)
{
	throw std::logic_error("The method or operation is not implemented.");
}

//=================================================================================
void C_VkRenderer::SortCommands()
{
	throw std::logic_error("The method or operation is not implemented.");
}

//=================================================================================
void C_VkRenderer::ExtractData()
{
	throw std::logic_error("The method or operation is not implemented.");
}

//=================================================================================
void C_VkRenderer::TransformData()
{
	throw std::logic_error("The method or operation is not implemented.");
}

//=================================================================================
void C_VkRenderer::Commit() const
{
	for (auto& command : (*m_CommandQueue)) {
		command->Commit();
	}
}

//=================================================================================
void C_VkRenderer::ClearCommandBuffers()
{
	m_CommandQueue->clear();
}

}
