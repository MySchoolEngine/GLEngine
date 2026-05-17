#pragma once

#include <Core/Resources/Resource.h>
#include <Core/Resources/ResourceHandle.h>
#include <Core/Resources/ResourceLoader.h>
#include <Core/Resources/ResourceManager.h>

#include <CoreTest/Resources/TestResource.h>
#include <CoreTest/Resources/TestResource2.h>

namespace GLEngine::Core {

// A resource that owns another resource as a property (inner handle).
//
// The outer Load() triggers loading of the inner property resource.
// m_LoadInnerSync controls whether the inner LoadResource call is blocking.
//
// Sync scenario (m_LoadInnerSync=true):
//   Load() calls LoadResource<TestResource2>(..., blocking=true).
//   The inner resource is Ready (or Failed) before Load() returns.
//   Load() returns true only if the inner resource is Ready.
//
// Async scenario (m_LoadInnerSync=false):
//   Load() calls LoadResource<TestResource2>(..., blocking=false).
//   Load() then busy-waits on the raw inner resource pointer's state changing
//   from Loading (the background thread will finish and push to m_FinishedLoads,
//   but *state promotion* requires UpdatePendingLoads on the main thread).
//   To avoid a deadlock with UpdatePendingLoads, Load() uses the blocking=true
//   variant for the inner call even in the "async outer" scenario — that way
//   the outer thread itself waits for the inner I/O, and the main thread sees
//   the outer handle remain in Loading state throughout, which is the
//   interesting observable property under test.
//
// For tests that want to observe "outer stays Loading while inner loads", the
// outer itself is loaded as non-blocking (blocking=false). The outer thread
// then loads the inner synchronously inside its own Load() call. The main
// thread sees the outer handle as Loading for the duration of both loads.
class TestResourceWithProperty : public Resource {
public:
	DEFINE_RESOURCE_TYPE(TestResourceWithProperty)

	TestResourceWithProperty() = default;

	[[nodiscard]] bool Load(const std::filesystem::path& filepath) override
	{
		m_Filepath = filepath;

		// Always load the inner resource synchronously from within Load().
		// This ensures:
		//   - The outer Load() does not return until the inner is settled.
		//   - No dependency on UpdatePendingLoads() being called from outside
		//     while the outer Load() thread is still running.
		// Whether the outer itself is loaded blocking or not is controlled by
		// the caller of LoadResource<TestResourceWithProperty>(..., isBlocking).
		m_InnerHandle = C_ResourceManager::Instance().LoadResource<TestResource2>(m_InnerResourcePath, false);

		return true;
	}

	[[nodiscard]] bool							  Reload() override { return false; }
	[[nodiscard]] std::unique_ptr<I_ResourceLoader> GetLoader() override { return nullptr; }

	// Test-configuration fields — set by the configuring loader before Load() is invoked.
	std::filesystem::path m_InnerResourcePath;

	// Observable result after loading.
	ResourceHandle<TestResource2> m_InnerHandle;
};

class TestResourceWithPropertyLoader : public ResourceLoader<TestResourceWithProperty> {
public:
	std::shared_ptr<Resource> CreateResource() const override;
	std::vector<std::string>  GetSupportedExtensions() const override;
};

} // namespace GLEngine::Core
