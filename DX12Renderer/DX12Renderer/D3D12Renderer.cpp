#include <DX12RendererStdafx.h>

#include <DX12Renderer/D3D12Renderer.h>

#include <Renderer/IRenderBatch.h>
#include <Renderer/IRenderCommand.h>

#include <Utils/DebugBreak.h>

namespace GLEngine::DX12Renderer {

//=================================================================================
C_D3D12Renderer::C_D3D12Renderer()
	: m_CommandQueue(new std::remove_pointer<decltype(m_CommandQueue)>::type)
{
}

//=================================================================================
C_D3D12Renderer::~C_D3D12Renderer()
{
	delete m_CommandQueue;
}

//=================================================================================
void C_D3D12Renderer::Lock(bool lock /*= true*/)
{
	m_Locked = lock;
}

//=================================================================================
void C_D3D12Renderer::AddCommand(T_CommandPtr command)
{
	if (m_Locked)
	{
		GL_DebugBreak();
	}
	m_CommandQueue->emplace_back(std::move(command));
}

//=================================================================================
void C_D3D12Renderer::Commit() const
{
	for (auto& command : (*m_CommandQueue))
	{
		command->Commit();
	}
}

//=================================================================================
void C_D3D12Renderer::ClearCommandBuffers()
{
	m_CommandQueue->clear();
}

//=================================================================================
Renderer::E_PassType C_D3D12Renderer::GetCurrentPassType() const
{
	return Renderer::E_PassType::FinalPass;
}

//=================================================================================
void C_D3D12Renderer::AddTransferCommand(T_CommandPtr)
{
	throw std::logic_error("The method or operation is not implemented.");
}

//=================================================================================
Renderer::I_Device& C_D3D12Renderer::GetDevice()
{
	throw std::logic_error("The method or operation is not implemented.");
}

//=================================================================================
void C_D3D12Renderer::SetBufferData(Renderer::Handle<Renderer::Buffer> dstBuffer, std::size_t numBytes, const void* data)
{
	throw std::logic_error("The method or operation is not implemented.");
}

//=================================================================================
Renderer::ResouceManager& C_D3D12Renderer::GetRM()
{
	throw std::logic_error("The method or operation is not implemented.");
}

} // namespace GLEngine::DX12Renderer
