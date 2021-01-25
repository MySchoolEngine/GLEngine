#pragma once

namespace GLEngine::GLRenderer::Commands {
//=================================================================================
template <class futureT, class retT, class FuturePolicy> void C_FutureCommand<futureT, retT, FuturePolicy>::Commit()
{
	Check(m_Future);
	if constexpr (std::is_same_v<void, retT>)
	{
		m_fnc(m_Future.get());
		m_Promise.set_value();
	}
	else
	{
		m_Promise.set_value(m_fnc(m_Future.get()));
	}
}

//=================================================================================
template <class futureT, class retT, class FuturePolicy> std::string C_FutureCommand<futureT, retT, FuturePolicy>::GetDescriptor() const
{
	return fmt::format("Future for {}", m_Name);
}
} // namespace GLEngine::GLRenderer::Commands
