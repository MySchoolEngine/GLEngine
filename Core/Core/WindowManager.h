#pragma once

#include <Core/IWindowManager.h>

#include <Utils/CompileTime/CompileTimeUtils.h>

#include <array>
#include <memory>
#include <numeric>
#include <type_traits>

namespace GLEngine::Core {
template <class... Managers> class C_WindwoManager : public I_WindowManager {
public:
	C_WindwoManager(C_Application::EventCallbackFn callback)
		: I_WindowManager(callback)
		, m_UpdatingManager(nullptr)
	{
	}

	//=================================================================================
	virtual ~C_WindwoManager() = default;

	//=================================================================================
	template <class T, typename = std::enable_if_t<std::disjunction_v<std::is_same<T, Managers>...>>> void AddManager(T* manager)
	{
		m_Managers[T_IndexOfType::IndexOf<T, Managers...>()] = std::unique_ptr<I_WindowManager>(manager);
	}

	//=================================================================================
	[[nodiscard]] virtual std::shared_ptr<I_Window> OpenNewWindow(const S_WindowInfo& info) override
	{
		for (auto& mgr : m_Managers)
		{
			if (!mgr)
				continue;
			if (auto window = mgr->OpenNewWindow(info))
			{
				return window;
			}
		}
		return nullptr;
	}


	//=================================================================================
	[[nodiscard]] virtual std::shared_ptr<I_Window> GetWindow(GUID guid) const override
	{
		for (auto& mgr : m_Managers)
		{
			if (!mgr)
				continue;
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
				m_UpdatingManager = manager.get();
				manager->Update();
			}
		}
	}

	//=================================================================================
	[[nodiscard]] virtual unsigned int NumWindows() const override
	{
		return std::accumulate(m_Managers.begin(), m_Managers.end(), 0,
							   [](unsigned int val, const auto& manager) -> unsigned int { return val + (manager == nullptr ? 0 : manager->NumWindows()); });
	}


	//=================================================================================
	[[nodiscard]] virtual const std::unique_ptr<GLEngine::Renderer::I_Renderer>& GetActiveRenderer() const override { return m_UpdatingManager->GetActiveRenderer(); }


	//=================================================================================
	virtual void OnEvent(Core::I_Event& event) override
	{
		for (const auto& manager : m_Managers)
		{
			if (manager)
			{
				m_UpdatingManager = manager.get();
				manager->OnEvent(event);
			}
		}
	}

private:
	std::array<std::unique_ptr<I_WindowManager>, sizeof...(Managers)> m_Managers;
	I_WindowManager*												  m_UpdatingManager;
};
} // namespace GLEngine::Core