
# Change Log
All notable changes to this project will be documented in this file.

## [Unreleased] - 02-02-2026
### Added
- Perlin noise generator for iamge editor

## [Unreleased] - 02-02-2026
### Added
- Move `C_GeomComponent` to the new rendering API [#285](/../../issues/285)

## [Unreleased] - 28-12-2025
### Added
- Support for multiple materials in `C_StaticMeshHandles` [#261](/../../issues/261)
- Load materials with textures from mesh resources [#262](/../../issues/262)
### Fixed
- Materials in [Stylized Nature Mega Kit[Standard]](data/Models/Stylized%20Nature%20MegaKit[Standard])

## [Unreleased] - 27-12-2025
### Added
- Gizmo for selected entity [#272](/../../issues/272)

## [Unreleased] - 26-12-2025
### Updates
- Switched to C++23
- Updated premake5 to 5.0.0-beta7
### Added
- AABB editor #270
- Disable for directiosn in gizmo
### Fixed
- Reflection GUI on non registered enums show asserts

## [Unreleased] - 05-12-2025
### Fixed
- Buildable resrource now tries to load if possible
- Heatmap in Ray trace window visible
- Bump OpenGL 4.4 -> 4.5 to make it work on integrated graphics

## [Unreleased] - 05-12-2025
### Added
- UV Coordinates to BVH
- Static mesh handles now handles the change of mesh asset
- Stylized Nature MegaKit[Standard] - asset pack
### Fixed
- AABB Serialization
- Load of buildable resource without loading parent one
- Default value applied on deserialize for Atomics
### Improvements
- AABB Size smaller by 1/3

## [Unreleased] - 01-11-2025
### Added
- Image editor radial wave generation
- Image editor file saving
### Fixed
- Brick generator GUI

## [Unreleased] - 14-10-2025
### Added
- Added support for std::set for serialize/deserialize

## [Unreleased] - 12-10-2025
### Fixed
- Resource manager with buildable resources

## [Unreleased] - 04-10-2025
### Added
- Handles rendering support
### Fixed
- Serialization
### Removed
- Old static mesh rendering


## [Unreleased] - 14-06-2025
### Added
- `Editor::C_ImageEditor` to Vulkan configuration
### Fixed
- `C_VkRenderer::SetTextureData` reflects number of channels
- `C_VkTexture` caching GUI handle
- `GUI::C_Windwow` correct destruction

## [Unreleased] - 13-02-2025

Goal of this branch was to create water simulation. Unfortunatelly I have faild and want to move on. One day I will be hopefully able to continue.

### Added
- `GLEngine::GLRenderer::C_WaterRendering` Water simulator
- `GLEngine::Renderer::C_PhysicsRenderer` Rendering for 2D physics
- `GLEngine::Renderer::IndirectDraw`
- `GLEngine::Renderer::IndirectDrawIndexed`

### Changed
- `GLEngine::Renderer::C_ImageViewer`
    - `SetBackground()`
    - `AddOverlay()`
    - `ShowLayers()` to show layers in sidebar
        - Possibility to hide image or overlay

### Fixed
- Keep shader name when hot reload
- `GLEngine::Renderer::MeshData::C_Geometry::CreatePlane` Now generates correct plane.
- `GLEngine::Renderer::C_CPURasterizer::DrawCircle` Correct blending of the alpha channel.
- `GLEngine::Renderer::C_CPURasterizer::ScanLineFloodFill` Correct blending of the alpha channel.
- `GLEngine::Renderer::C_CPURasterizer::BresenhamHorizontal` Correct blending of the alpha channel.
- `GLEngine::Renderer::C_CPURasterizer::XiaolinWu` Correct blending of the alpha channel.
- Fixed some typos across engine.

## [Unreleased] - 22-12-2024
### Added
- `GLEngine::GUI::C_ResourceDialogWindow` generic resource file dialog

## [Unreleased] - 17-11-2024

This version started long ago and aimed for more API agnostic approach to rendering. Aiming for rendering through handles and allow me to optimase the rendering in the future without writing API specific code.

### Added

- `GLEngine::Renderer::ResouceManager` to manage GPU resources.
- `Utils::Reflection::MetaGUI::Texture` first example how to write GUI for `GLEngine::Core::ResourceHandle<T>`
- `GLEngine::Core::Filesystem` for usual path used across engine for unification

### Changed

- `GLEngine::Renderer::C_Material` GUI now allows setting textures
- `GLEngine::Editor::C_ImageEditor` works with handles
- `GLEngine::Core::C_ResourceManager::s_UpdatesBeforeDelete` timeout to unload unused resources
