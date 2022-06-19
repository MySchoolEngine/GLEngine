#pragma once

#include <functional>

namespace Utils
{
// Simple RAII finalizer for given scope
class C_ScopeFinalizer {
public:
	explicit C_ScopeFinalizer(const std::function<void()>& finalizer)
		: m_Finalizer(finalizer)
	{}
	~C_ScopeFinalizer() { m_Finalizer(); }
  private:
	std::function<void()> m_Finalizer;
};
}

