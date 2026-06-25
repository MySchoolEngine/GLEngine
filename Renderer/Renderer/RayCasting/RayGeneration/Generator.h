#pragma once

#include <Renderer/RendererApi.h>

#include <coroutine>
#include <utility>

namespace GLEngine::Renderer {

template <typename T> class RENDERER_API_EXPORT Generator {
public:
	struct promise_type {
		T current_value;

		Generator			get_return_object() noexcept { return Generator{std::coroutine_handle<promise_type>::from_promise(*this)}; }
		std::suspend_always initial_suspend() noexcept { return {}; }
		std::suspend_always final_suspend() noexcept { return {}; }
		std::suspend_always yield_value(T value) noexcept(std::is_nothrow_move_assignable_v<T>)
		{
			current_value = std::move(value);
			return {};
		}
		void			  return_void() noexcept {}
		[[noreturn]] void unhandled_exception() noexcept { std::terminate(); }
	};

	Generator(Generator&& other) noexcept
		: m_Handle(std::exchange(other.m_Handle, nullptr))
	{
	}
	Generator(const Generator&) = delete;
	Generator& operator=(const Generator&) = delete;
	~Generator()
	{
		if (m_Handle)
			m_Handle.destroy();
	}

	struct iterator {
		std::coroutine_handle<promise_type> handle;

		bool	  operator==(std::default_sentinel_t) const noexcept { return !handle || handle.done(); }
		iterator& operator++()
		{
			handle.resume();
			return *this;
		}
		T&		 operator*() noexcept { return handle.promise().current_value; }
		const T& operator*() const noexcept { return handle.promise().current_value; }
	};

	iterator begin()
	{
		if (m_Handle)
			m_Handle.resume();
		return {m_Handle};
	}
	std::default_sentinel_t end() noexcept { return {}; }

private:
	explicit Generator(std::coroutine_handle<promise_type> h) noexcept
		: m_Handle(h)
	{
	}
	std::coroutine_handle<promise_type> m_Handle;
};

} // namespace GLEngine::Renderer
