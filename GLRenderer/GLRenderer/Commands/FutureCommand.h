#pragma once

#include <Renderer/IRenderCommand.h>

#include <future>
#include <string>

namespace GLEngine::GLRenderer::Commands {
class FuturePolicyNotBlocking;
class FuturePolicyBlocking;

//=================================================================================
template<class futureT, class retT = void, class FuturePolicy = FuturePolicyNotBlocking>
class C_FutureCommand : public Renderer::I_RenderCommand, private FuturePolicy
{
public:
	C_FutureCommand(std::future<futureT>&& future, std::function<retT(futureT)> fnc, const std::string& name)
		: m_Future(std::move(future))
		, m_fnc(fnc)
		, m_Name(name) {}
	//======================================
	// Renderer::I_RenderCommand
	//======================================
	virtual void Commit() override;
	virtual E_Type GetType() const override { return E_Type::Undefined; }
	virtual std::shared_ptr<Renderer::I_Resource> GetResource() const override { return nullptr; }
	[[nodiscard]] virtual std::string GetDescriptor() const override;

	[[nodiscard]] std::future<retT> GetFuture() { return m_Promise.get_future(); }

	template<class nextRet = void, class nextPolicy = FuturePolicy>
	[[nodiscard]] std::unique_ptr<C_FutureCommand<retT, nextRet, nextPolicy>> Connect(std::function<nextRet(retT)> fnc, std::string&& name)
	{
		return std::make_unique<C_FutureCommand<retT, nextRet, nextPolicy>>(GetFuture(), fnc, std::move(name));
	}
private:
	std::future<futureT>			m_Future;
	std::promise<retT>				m_Promise;
	std::function<retT(futureT)>	m_fnc;
	std::string			m_Name;

	using FuturePolicy::Check;
};

//=================================================================================
class FuturePolicyNotBlocking
{
protected:
	template<class futureT>
	void Check(const std::future<futureT>& f)
	{
		GLE_ASSERT(f.wait_for(std::chrono::seconds(0)) == std::future_status::ready, "Future command is blocking render thread");
	}
};

//=================================================================================
class FuturePolicyBlocking
{
protected:
	template<class futureT>
	void Check(const std::future<futureT>& f)
	{
		// skip the check
	}
};

//=================================================================================
template<class futureT, class retT = void>
using T_FutureCommandNonBlock = C_FutureCommand<futureT, retT, FuturePolicyNotBlocking>;

template<class futureT, class retT = void>
using T_FutureCommandCanBlock = C_FutureCommand<futureT, retT, FuturePolicyBlocking>;

//=================================================================================
#ifdef GL_ENGINE_DEBUG
template<class futureT, class retT = void>
using T_FutureCommand = T_FutureCommandNonBlock<futureT, retT>;
#else
template<class futureT, class retT = void>
using T_FutureCommand = T_FutureCommandCanBlock<futureT, retT>;
#endif

}
#include <GLRenderer/Commands/FutureCommand.inl>
