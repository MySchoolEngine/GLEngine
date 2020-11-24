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
	if (m_Locked) {
		DebugBreak();
	}
	m_CommandQueue->emplace_back(std::move(command));
}

//=================================================================================
void C_D3D12Renderer::AddBatch(T_BatchPtr)
{
	throw std::logic_error("The method or operation is not implemented.");
}

//=================================================================================
void C_D3D12Renderer::SortCommands()
{
	throw std::logic_error("The method or operation is not implemented.");
}

//=================================================================================
void C_D3D12Renderer::ExtractData()
{
	throw std::logic_error("The method or operation is not implemented.");
}

//=================================================================================
void C_D3D12Renderer::TransformData()
{
	throw std::logic_error("The method or operation is not implemented.");
}

//=================================================================================
void C_D3D12Renderer::Commit() const
{
	for (auto& command : (*m_CommandQueue)) {
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

}
