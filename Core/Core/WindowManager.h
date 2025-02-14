#pragma once

#include <Core/IWindowManager.h>

#include <Utils/CompileTime/CompileTimeUtils.h>

#include <array>
#include <memory>
#include <numeric>
#include <type_traits>

namespace GLEngine::Core {
template <class... Managers> class C_WindowManager final : public I_WindowManager {
public:
	explicit C_WindowManager(const C_Application::EventCallbackFn& callback)
		: I_WindowManager(callback)
		, m_UpdatingManager(nullptr)
	{
	}
	C_WindowManager(const C_WindowManager& other)				 = default;
	C_WindowManager(C_WindowManager&& other) noexcept			 = default;
	C_WindowManager& operator=(const C_WindowManager& other)	 = default;
	C_WindowManager& operator=(C_WindowManager&& other) noexcept = default;

	//=================================================================================
	~C_WindowManager() override = default;

	//=================================================================================
	template <class T, typename = std::enable_if_t<std::disjunction_v<std::is_same<T, Managers>...>>> void AddManager(T* manager)
	{
		m_Managers[T_IndexOfType::IndexOf<T, Managers...>()] = std::unique_ptr<I_WindowManager>(manager);
	}

	//=================================================================================
	[[nodiscard]] std::shared_ptr<I_Window> OpenNewWindow(const S_WindowInfo& info) override
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
	[[nodiscard]] std::shared_ptr<I_Window> GetWindow(GUID guid) const override
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
	void Update() override
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
	[[nodiscard]] unsigned int NumWindows() const override
	{
		return std::accumulate(m_Managers.begin(), m_Managers.end(), 0,
							   [](unsigned int val, const auto& manager) -> unsigned int { return val + (manager == nullptr ? 0 : manager->NumWindows()); });
	}

	//=================================================================================
	[[nodiscard]] Renderer::I_Renderer* ActiveRendererPtr() override
	{
		if (!m_UpdatingManager)
		{
			return nullptr;
		}
		return m_UpdatingManager->ActiveRendererPtr();
	}

	//=================================================================================
	Renderer::I_Renderer& GetActiveRenderer() override
	{
		GLE_ASSERT(m_UpdatingManager, "Getting renderer outside of update!");
		return m_UpdatingManager->GetActiveRenderer();
	}


	//=================================================================================
	void OnEvent(I_Event& event) override
	{
		auto updateManager = m_UpdatingManager;
		for (const auto& manager : m_Managers)
		{
			if (manager)
			{
				m_UpdatingManager = manager.get();
				manager->OnEvent(event);
			}
		}
		m_UpdatingManager = updateManager;
	}

private:
	std::array<std::unique_ptr<I_WindowManager>, sizeof...(Managers)> m_Managers;
	I_WindowManager*												  m_UpdatingManager;
};
} // namespace GLEngine::Core