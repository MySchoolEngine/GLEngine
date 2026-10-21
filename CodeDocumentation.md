# Documentation


## Registration
Due to heavy usage of clang tidy I highly recommend wrapping any RTTR registration block in those comments.

```cpp
// clang-format off
RTTR_REGISTRATION
{
...
}
// clang-format on
```

## Serialization

To compress XML data we have number of strategies. For some simple types we support inline serialization into an attribute or inner XML data.

```cpp
bool SerializeString(const glm::vec3& vec, std::string& ret)
{
	ret = "(" + std::to_string(vec.x) + "," + std::to_string(vec.y) + "," + std::to_string(vec.z) + ")";
	return true;
}

bool DeserializeString(const std::string& str, glm::vec3& vec)
{
	if (str[0] != '(' || str[str.length() - 1] != ')')
		return false;
	std::stringstream ss(str.substr(1, str.length() - 2));

	return DeserializeFloatList(ss, vec);
}

#include <Utils/Serialization/SerializationUtils.h>

// clang-format off
RTTR_REGISTRATION
{
	REGISTER_SERIALIZATION(glm::vec3);
}
// clang-format on
```

For often repeated default values we can also define default values. They won't be serialized, and on deserialization the property will be set to the given value.

```cpp
.property("Transform", &C_Trimesh::m_Transform)(REGISTER_DEFAULT_VALUE(glm::mat4(1.f)))
```

Sometimes we want to register some properties but not to serialize them when saving into XML. We allow such behaviour using metadata.

Example shows property used for GUI but it is being saved through other property.
```cpp
.property("Rotation", &C_Transformations::m_Rotation)
(
	rttr::policy::prop::as_reference_wrapper, 
	RegisterMetaclass<MetaGUI::Vec3>(), 
	RegisterMetamember<UI::Vec3::Name>("Rotation"),
	RegisterMetamember<SerializationCls::NoSerialize>(true)
)
```

#### Ignoring default values
If you want to avoid serialization of default values put `REGISTER_DEFAULT_VALUE(GLEngine::Core::ResourceHandle<TextureResource>())` to your metadata. This requires implementation of a valid `operator==` and a call to `rttr::type::register_equal_comparator`.

## Entity Component System

Entities are `entt::entity` handles owned by a `GLEngine::Entity::C_World` (a thin wrapper around an `entt::registry`). Components are plain structs with no base class and no virtual functions; systems are free functions that operate on a `C_World` each frame. There is no scheduler that auto-runs systems - each window's `Update()` calls them manually, in an order it controls itself (see "Adding a system" below).

### Adding a component

A component is a plain struct, RTTR-registered only if it needs XML (de)serialization or an Editor property panel, and listed in `C_ComponentRegistry` only if you want the Editor's "Add component"/property panel to see it at all.

1. **Define the struct.** No base class, no virtuals - just data. `S_TransformComponent` (`Entity/Entity/Components/CoreComponents.h`):
   ```cpp
   struct S_TransformComponent {
       glm::vec3 translation{0.f, 0.f, 0.f};
       glm::quat rotation{1.f, 0.f, 0.f, 0.f};
       glm::vec3 scale{1.f, 1.f, 1.f};

       RTTR_REGISTRATION_FRIEND
   };
   ```
   `RTTR_REGISTRATION_FRIEND` is only needed if a later step registers private members; `S_TransformComponent`'s are public, so it's there for consistency with sibling components, not a hard requirement.

2. **RTTR-register it**, if you want it serialized or drawn - skip this step entirely for a component that's purely runtime-internal (e.g. a GPU-handle cache like `S_StaticMeshGPU`) and never needs either:
   ```cpp
   // clang-format off
   RTTR_REGISTRATION
   {
       using namespace GLEngine::Entity;
       rttr::registration::class_<S_TransformComponent>("TransformComponent")
           .constructor<>()(rttr::policy::ctor::as_object)
           .property("Translation", &S_TransformComponent::translation)
           .property("Rotation", &S_TransformComponent::rotation)(REGISTER_DEFAULT_VALUE(glm::quat(1.f, 0.f, 0.f, 0.f)))
           .property("Scale", &S_TransformComponent::scale)(REGISTER_DEFAULT_VALUE(glm::vec3(1.f, 1.f, 1.f)));
   }
   // clang-format on
   ```
   Property names here (`"Translation"`, `"Rotation"`, `"Scale"`) are the exact XML attribute names `C_WorldSerializer` reads/writes - see the "Serialization" section above for `rttr::policy::prop::as_reference_wrapper`, default values, and `NoSerialize` metadata if a property needs GUI-only or no-serialize treatment.

3. **Register it with `C_ComponentRegistry`**, if you want the Editor to know about it (component browsing, "Add component", the property panel, save/load through `C_WorldSerializer`). A component used only by internal systems (never authored in a level, never shown in the Editor) can skip this too.
   ```cpp
   namespace {
   const bool g_RegisterTransformComponent = [] {
       GLEngine::Entity::C_ComponentRegistry::Instance().Register<GLEngine::Entity::S_TransformComponent>(
           "Transform", /*serializable=*/true, /*userAddable=*/false, /*drawGUI=*/true);
       return true;
   }();
   } // namespace
   ```
   The static-initializer-lambda idiom (register inside a file-scope `const bool` lambda) is required, not stylistic - this is what makes registration happen automatically before `main()`, with no explicit call site anywhere. The string (`"Transform"`) is the name `C_WorldSerializer` uses as the XML tag and the Editor uses as the display name - it does not have to match the C++ type name (compare `S_TransformComponent` -> `"Transform"` vs `S_PointLight` -> `"PointLight"`). `userAddable=false` hides the "Remove" button and excludes it from the "Add component" list - use that for components every entity already gets unconditionally (like `Transform`), not for anything a level author chooses to attach.

   **Gotcha:** if your component's `.cpp` has *only* this registration side effect (no function anyone calls directly), and it lives in a project that's ever linked as a static library (check for a `*Static` premake project, e.g. `RendererStatic`, used by test/benchmark executables), the linker can silently drop that whole translation unit since nothing references a symbol in it - the registration then never runs and the component mysteriously "doesn't exist" only in those binaries. Fix it by exporting a no-op function from the same file (e.g. `EnsureLightComponentsRegistered()` in `LightComponents.cpp`) and calling it once from that static-lib consumer's test/benchmark setup, purely to force the linker to keep the translation unit.

A second, real example with two components in one file (`Renderer/Renderer/Components/Lights/LightComponents.cpp`):
```cpp
// clang-format off
RTTR_REGISTRATION
{
    using namespace GLEngine::Renderer;
    rttr::registration::class_<S_PointLight>("PointLight")
        .constructor<>()(rttr::policy::ctor::as_object)
        .property("Intensity", &S_PointLight::intensity)
        .property("Color", &S_PointLight::color);
}
// clang-format on

namespace {
const bool g_RegisterLightComponents = [] {
    using namespace GLEngine::Entity;
    C_ComponentRegistry::Instance().Register<GLEngine::Renderer::S_PointLight>("PointLight");
    return true;
}();
} // namespace
```

### Adding a system

A system is a free function (or, if it needs to track its own state across frames, a small namespace of free functions - see `Renderer/Renderer/Systems/StaticMeshSystem.h`'s `Update`/`Render`/`RegisterDestroyHook`) taking a `C_World&` and doing its work by iterating an `entt::view`. There is no central scheduler - each window that owns a `C_World` calls systems manually, once per frame, in an order *it* chooses and is responsible for getting right.

1. **Write the system.** `TransformSystem::Update` (`Entity/Entity/Systems/TransformSystem.h`/`.cpp`) is the simplest real example - it only recomputes entities tagged dirty, not everything every frame:
   ```cpp
   namespace GLEngine::Entity::TransformSystem {
   void Update(C_World& world);
   }
   ```
   A system that reads components and writes a different component back, like `LightSystem::Update` (`Renderer/Renderer/Systems/LightSystem.h`/`.cpp`), looks like:
   ```cpp
   namespace GLEngine::Renderer::LightSystem {
   void Update(Entity::C_World& world);
   }
   ```
   ```cpp
   void Update(Entity::C_World& world)
   {
       auto& registry = world.Registry();
       for (auto [e, light, worldTransform] : registry.view<S_PointLight, Entity::S_WorldTransformComponent>().each())
       {
           registry.emplace_or_replace<Entity::S_LocalBounds>(e, GetAABB(light, worldTransform));
       }
       // ... same pattern repeated for S_AreaLight, S_SunLight
   }
   ```

2. **Call it from every window's per-frame `Update()`, in the right position.** This is the part a scheduler would normally handle, and the part you have to get right by hand instead - look at an existing window (`GLRenderer/GLRenderer/Windows/ExperimentWindow.cpp`'s `Update()`) for the real, current order before adding a new call, since getting the order wrong is a silent bug, not a compile error:
   ```cpp
   m_World.OnUpdate(); // flushes deferred DestroyEntity() calls first
   Entity::TransformSystem::Update(m_World); // must run before anything reads S_WorldTransformComponent
   Renderer::LightSystem::Update(m_World); // depends on fresh world transforms
   Renderer::StaticMeshSystem::Update(m_World, *m_renderer);
   Renderer::GeomSystem::Update(m_World, *m_renderer);
   Renderer::CameraSystem::Update(m_World);
   ```
   A system that (like `LightSystem`, `StaticMeshSystem`) depends on up-to-date `S_WorldTransformComponent` must be called after `TransformSystem::Update`, same as every other call site. There's no single place that enforces this - each window's `Update()` has to be updated by hand. The two windows that currently own a `C_World` don't run the same set of systems: `ExperimentWindow` runs all of `TransformSystem`/`LightSystem`/`StaticMeshSystem`/`GeomSystem`/`CameraSystem`, while `VkWindow` only runs `TransformSystem` and `StaticMeshSystem` (no lighting in that path yet). When adding a new system, check each window that owns a `C_World` individually and decide whether it needs your system - don't assume one window's call list applies to the other.