#pragma once

#include <Core/Resources/Resource.h>
#include <Core/Resources/ResourceHandle.h>
#include <Core/Resources/ResourceLoader.h>
#include <Core/Resources/ResourceManager.h>

#include <CoreTest/Resources/TestClasses/DelayFileTestResource.h>

namespace GLEngine::Core {

// Same shape as TestResourceWithProperty, but the inner property resource is
// DelayFileTestResource (a real ~100ms Load() delay, see
// DelayFileTestResource::s_LoadTime) instead of TestResource2 (which loads
// instantly). Use this when a test needs a reliable, non-racy window during
// which the inner resource is observably still Loading - e.g. to prove the
// outer handle does not report Ready (or does not read inner data) while the
// inner load is still in flight, rather than relying on sleep/poll timing
// against an inner resource that settles too fast to reliably observe.
//
// The outer Load() triggers loading of the inner property resource.
//
// Sync scenario (outer loaded blocking=true):
//   Load() calls LoadResource<DelayFileTestResource>(..., blocking=true).
//   The inner resource is Ready (or Failed) before Load() returns.
//
// Async scenario (outer loaded blocking=false):
//   Load() calls LoadResource<DelayFileTestResource>(..., blocking=ctx.m_isBlocking),
//   i.e. the inner call mirrors whatever blocking mode the outer itself was
//   requested with. Load() runs on the background thread the outer load
//   spawned, so the inner ~100ms sleep happens on that same thread; the main
//   thread observes the outer handle as Loading for the duration of both.
class TestResourceWithDelayedProperty : public Resource {
public:
	DEFINE_RESOURCE_TYPE(TestResourceWithDelayedProperty)

	TestResourceWithDelayedProperty() = default;

	[[nodiscard]] bool Load(const std::filesystem::path& filepath, LoadCtx& ctx) override;

	[[nodiscard]] bool								Reload() override { return false; }
	[[nodiscard]] std::unique_ptr<I_ResourceLoader> GetLoader() override { return nullptr; }

	// Test-configuration field — set by the configuring loader before Load() is invoked.
	std::filesystem::path m_InnerResourcePath;

	// Observable result after loading.
	ResourceHandle<DelayFileTestResource> m_InnerHandle;
};

class TestResourceWithDelayedPropertyLoader : public ResourceLoader<TestResourceWithDelayedProperty> {
public:
	std::shared_ptr<Resource> CreateResource() const override;
	std::vector<std::string>  GetSupportedExtensions() const override;
};

} // namespace GLEngine::Core
