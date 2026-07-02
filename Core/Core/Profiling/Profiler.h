#pragma once

#ifdef TRACY_ENABLE
	#include <tracy/Tracy.hpp>
	#define GL_PROFILE_SCOPE()			 ZoneScoped
	#define GL_PROFILE_SCOPE_N(name)	 ZoneScopedN(name)
	#define GL_PROFILE_FRAME()			 FrameMark
	#define GL_PROFILE_FRAME_N(name)	 FrameMarkNamed(name)
	#define GL_PROFILE_THREAD_NAME(name) tracy::SetThreadName(name)
#else
	#define GL_PROFILE_SCOPE()
	#define GL_PROFILE_SCOPE_N(name)
	#define GL_PROFILE_FRAME()
	#define GL_PROFILE_FRAME_N(name)
	#define GL_PROFILE_THREAD_NAME(name)
#endif

// ========================================================
// Counter macros
// ========================================================
#ifdef TRACY_ENABLE
// example of counters definition
// #define LIST_OF_COUNTERS(DO)                                                                                                                                                    \
	// 	DO(RayAABBChecks)                                                                                                                                                          \
	// 	DO(RayAABBRejected)                                                                                                                                                        \
	// 	DO(RayTriangleChecks)

	#define Detail_DefineTracyCounter(name) thread_local int64_t tl_##name = 0;
	#define Detail_ResetTracyCounter(name)	tl_##name = 0;
	#define Detail_SendTracyCounter(name)	TracyPlot(#name, tl_##name);
	#define Detail_PlotTracyCounter(name)	TracyPlotConfig(#name, tracy::PlotFormatType::Number, true, true, 0);

	#define ResetTracyCounters(LIST) LIST(Detail_ResetTracyCounter) static_assert(true, "")
	#define SendTracyCounters(LIST)	 LIST(Detail_SendTracyCounter) static_assert(true, "")
	#define DeclareTracyCounters(LIST)                                                                                                                                             \
		namespace {                                                                                                                                                                \
		LIST(Detail_DefineTracyCounter)                                                                                                                                            \
		} /* namespace */                                                                                                                                                          \
		static_assert(true, "")
	#define PlotTracyCounters(LIST)                                                                                                                                                \
		[[maybe_unused]] static const bool s_PlotsConfigured = []() {                                                                                                              \
			LIST(Detail_PlotTracyCounter)                                                                                                                                          \
			return true;                                                                                                                                                           \
		}()
#else


	#define ResetTracyCounters(LIST)   static_assert(true, "")
	#define SendTracyCounters(LIST)	   static_assert(true, "")
	#define DeclareTracyCounters(LIST) static_assert(true, "")
	#define DeclareTracyCounters(LIST) static_assert(true, "")
	#define PlotTracyCounters(LIST)	   static_assert(true, "")
#endif