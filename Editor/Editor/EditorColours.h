#pragma once

#include <Renderer/Colours.h>

namespace GLEngine::Renderer {
class MaterialResource;
class AnimationResource;
class TextureResource;
class MeshResource;
class C_TrimeshModel;
} // namespace GLEngine::Renderer


namespace GLEngine::Editor { namespace Colours {
using namespace GLEngine::Colours;
namespace Resources {
namespace detail {
constexpr float pastelIntensity = 0.4f;
}
template <class ResourceT> inline constexpr static T_Colour Colour = white;
// mesh
template <> inline constexpr T_Colour Colour<Renderer::C_TrimeshModel> = Pastel(blue, detail::pastelIntensity);
template <> inline constexpr T_Colour Colour<Renderer::MeshResource>   = Pastel(blue, detail::pastelIntensity);
// textures
template <> inline constexpr T_Colour Colour<Renderer::TextureResource> = Pastel(red, detail::pastelIntensity);
// animations
template <> inline constexpr T_Colour Colour<Renderer::AnimationResource> = Pastel(yellow, detail::pastelIntensity);
// materials
template <> inline constexpr T_Colour Colour<Renderer::MaterialResource> = Pastel(green, detail::pastelIntensity);
} // namespace Resources


namespace Editing {

inline constexpr static auto mouseOver = yellow;
inline constexpr static auto selected  = blue;
inline constexpr static auto disabled  = gray;
} // namespace Editing
}} // namespace GLEngine::Editor::Colours