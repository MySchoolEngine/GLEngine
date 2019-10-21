#pragma once

#include <Core/IWindowManager.h>

#include <Utils/CompileTime/CompileTimeUtils.h>

#include <type_traits>
#include <memory>
#include <numeric>
#include <array>

namespace GLEngine::Core {
template<class ...Managers>
class C_WindwoManager : public I_WindowManager
{
public:
	C_WindwoManager(C_Application::EventCallbackFn callback)
		: I_WindowManager(callback)
	{}

	virtual ~C_WindwoManager() = default;

	// template<class T, class ...Args, typename = std::enable_if_t< std::disjunction_v<std::is_same<T, Managers>...>>>
	// void AddManager(Args ...args) {
	// 	m_Managers[T_IndexOfType::IndexOf<T, Managers...>()] = std::make_unique<T>(std::forward<Args>()...);
	// }

	//=================================================================================
	template<class T, typename = std::enable_if_t< std::disjunction_v<std::is_same<T, Managers>...>>>
	void AddManager(T* manager)
	{
		m_Managers[T_IndexOfType::IndexOf<T, Managers...>()] = std::unique_ptr<I_WindowManager>(manager);
	}

	//=================================================================================
	virtual std::shared_ptr<I_Window> OpenNewWindow(const S_WindowInfo& info) override
	{
		for (auto& mgr : m_Managers)
		{
			if (!mgr) continue;
			if (auto window = mgr->OpenNewWindow(info))
			{
				return window;
			}
		}
		return nullptr;
	}


	//=================================================================================
	virtual std::shared_ptr<I_Window> GetWindow(GUID guid) const override
	{
		for (auto& mgr : m_Managers)
		{
			if(!mgr) continue;
			if (auto window = mgr->GetWindow(guid))
			{
				return window;
			}
		}
		return nullptr;
	}


	//=================================================================================
	virtual void Update() override
	{
		for (const auto& manager : m_Managers)
		{
			if (manager)
			{
				manager->Update();
			}
		}
	}

	//=================================================================================
	virtual unsigned int NumWindows() const override
	{
		unsigned int acc = 0;
		for (const auto& manager : m_Managers)
		{
			if (manager)
			{
				acc += manager->NumWindows();
			}
		}
		return acc;
	}

private:
	std::array<std::unique_ptr<I_WindowManager>, sizeof...(Managers)> m_Managers;
};
}