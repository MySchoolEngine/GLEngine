
# Change Log
All notable changes to this project will be documented in this file.


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
### Fixed