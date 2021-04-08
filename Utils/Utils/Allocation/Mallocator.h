#pragma once

#include <Utils/Allocation/Internal/AllocatorUtils.h>

#include <memory>

namespace Utils::Allocation {

template <std::size_t alignment = 4> class C_Mallocator {
public:
  using size_type = std::size_t;

  C_Mallocator() : m_head(m_memory) {}

  /************************************************************************/
  /* Allocates n bytes of memory.                                         */
  /************************************************************************/
  void *allocate(size_type n) { return malloc(AllocUtils::roundToAligned(n)); }

  template <class T> T *allocate() {
    return static_cast<T *>(allocate(sizeof(T)));
  }

  template <class T> T *allocateArray(size_type n) {
    return static_cast<T *>(allocate(sizeof(T) * n));
  }

  template <class T> void deallocate(T *p) { free(p); }

  template <class T> void deallocateArray(T *p, size_type n) { free(p); }

private:
  using AllocUtils =
      C_AllocatorUtils<C_StackAllocator<size, alignment>, alignment>;
};

} // namespace Utils::Allocation