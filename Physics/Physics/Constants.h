#pragma once

#include <Physics/Quantities.h>

namespace GLEngine::Physics::Constants {
constexpr auto g = Scalars::acceleration{9.8067f};
namespace Phys2D {
constexpr auto g = Physics::Phys2D::acceleration{0, -9.8067f};
}
}