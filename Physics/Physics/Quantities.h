#pragma once

#include <Physics/SpatialTypes.h>

#include <glm/glm.hpp>

#include <DULib/StrongType.h>

namespace GLEngine::Physics {
namespace Scalars {
using density	   = DULib::StrongType<float, struct density_tag>;		// kg/m^3
using volume	   = DULib::StrongType<float, struct volume_tag>;		// m^3
using velocity	   = DULib::StrongType<float, struct velocity_tag>;		// m/s
using time_type	   = DULib::StrongType<float, struct time_tag>;			// s
using acceleration = DULib::StrongType<float, struct acceleration_tag>; // m/s^2
using force		   = DULib::StrongType<float, struct force_tag>;		// N or kg*m/s^2
using mass		   = DULib::StrongType<float, struct mass_tag>;			// kg

//=================================================================================
// equalities
//=================================================================================
bool equals(density, density);
bool equals(volume, volume);
bool equals(velocity, velocity);
bool equals(time_type, time_type);
bool equals(acceleration, acceleration);
bool equals(force, force);
bool equals(mass, mass);

std::strong_ordering order(density, density);
std::strong_ordering order(volume, volume);
std::strong_ordering order(velocity, velocity);
std::strong_ordering order(time_type, time_type);
std::strong_ordering order(acceleration, acceleration);
std::strong_ordering order(force, force);
std::strong_ordering order(mass, mass);

//=================================================================================
// density
//=================================================================================
#pragma region -- density
// closures
auto divide(density, float) -> density;
// D = m/V
auto multiply(density, volume) -> mass;
auto divide(mass, volume) -> density;
auto divide(mass, density) -> volume;
#pragma endregion

//=================================================================================
// time_type
//=================================================================================
#pragma region -- density


// arithmetics
time_type multiply(time_type, float);
time_type divide(time_type, float);

// user-defined literals
namespace time_typeLiterals {
[[nodiscard]] constexpr time_type operator""_s(long double seconds)
{
	return time_type{static_cast<float>(seconds)};
}
[[nodiscard]] constexpr time_type operator""_m(long double minutes)
{
	return time_type{static_cast<float>(minutes) * 60.f};
}
} // namespace distanceLiterals
#pragma endregion
//=================================================================================
// velocity
//=================================================================================
#pragma region -- velocity
// closures
velocity add(velocity, velocity);
velocity subtract(velocity, velocity);
velocity negate(velocity);

// arithmetics
velocity multiply(velocity, float);
velocity divide(velocity, float);

// v = t * s
auto multiply(time_type, velocity) -> distance;
auto divide(distance, time_type) -> velocity;
auto divide(distance, velocity) -> time_type;
// v = a * t
auto multiply(acceleration, time_type) -> velocity;
auto divide(velocity, time_type) -> acceleration;
// F = m * a
auto multiply(mass, acceleration) -> force;
auto divide(force, acceleration) -> mass;
auto divide(force, mass) -> acceleration;
#pragma endregion

//=================================================================================
// acceleration
//=================================================================================
#pragma region -- acceleration
// closures
acceleration add(acceleration, acceleration);
acceleration subtract(acceleration, acceleration);
acceleration negate(acceleration);
#pragma endregion


//=================================================================================
// density
//=================================================================================
#pragma region-- density
// closures
auto multiply(mass, float) -> mass;
auto add(mass, mass) -> mass;
auto subtract(mass, mass) -> mass;
#pragma endregion


//=================================================================================
// velocity
//=================================================================================
#pragma region-- velocity
#pragma endregion
} // namespace Scalars
namespace Phys2D {
using velocity	   = DULib::StrongType<glm::vec2, struct velocity_2d_tag>;
using acceleration = DULib::StrongType<glm::vec2, struct acceleration_2d_tag>;
using force		   = DULib::StrongType<glm::vec2, struct force_2d_tag>;


//=================================================================================
// velocity
//=================================================================================
#pragma region-- velocity
// closures
velocity add(velocity, velocity);
velocity subtract(velocity, velocity);
velocity negate(velocity);

// v = t * s
auto multiply(Scalars::time_type, velocity) -> d_position;
auto multiply(velocity, Scalars::time_type) -> d_position;
auto divide(d_position, Scalars::time_type) -> velocity;
auto divide(d_position, velocity) -> Scalars::time_type;
// v = a * t
auto multiply(acceleration, Scalars::time_type) -> velocity;
auto divide(velocity, Scalars::time_type) -> acceleration;
#pragma endregion

//=================================================================================
// force
//=================================================================================
#pragma region-- force
// F = m * a
auto multiply(Scalars::mass, acceleration) -> force;
auto divide(force, acceleration) -> Scalars::mass;
auto divide(force, Scalars::mass) -> acceleration;
#pragma endregion

//=================================================================================
// density
//=================================================================================

} // namespace Phys2D
namespace Phys3D {

}
} // namespace GLEngine::Physics