#pragma once

#ifdef TRACY_ENABLE
  #include <tracy/Tracy.hpp>
  #define GL_PROFILE_SCOPE()             ZoneScoped
  #define GL_PROFILE_SCOPE_N(name)       ZoneScopedN(name)
  #define GL_PROFILE_FRAME()             FrameMark
  #define GL_PROFILE_FRAME_N(name)       FrameMarkNamed(name)
  #define GL_PROFILE_THREAD_NAME(name)   tracy::SetThreadName(name)
#else
  #define GL_PROFILE_SCOPE()
  #define GL_PROFILE_SCOPE_N(name)
  #define GL_PROFILE_FRAME()
  #define GL_PROFILE_FRAME_N(name)
  #define GL_PROFILE_THREAD_NAME(name)
#endif
