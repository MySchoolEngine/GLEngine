#include <RendererTestStdafx.h>

#include <Renderer/Cameras/OrbitalCamera.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace GLEngine::Renderer::Cameras {

TEST(OrbitalCamera, SetOwnerWorldTransform_AppliesOwnerRotationToTheLocalOffset)
{
	// Default ctor: zoom=5, angleX=90deg, angleY=0 -> the camera's own orbit offset (before the
	// owner's transform is folded in) is (0,0,5).
	C_OrbitalCamera camera;
	camera.SetComponentMatrix(glm::translate(glm::mat4(1.f), glm::vec3(1.f, 0.f, 0.f))); // local offset from owner

	const auto ownerRotation  = glm::angleAxis(glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
	const auto ownerTransform = glm::translate(glm::mat4(1.f), glm::vec3(10.f, 0.f, 0.f)) * glm::mat4_cast(ownerRotation);
	camera.SetOwnerWorldTransform(ownerTransform);
	camera.Update();

	// Before this fix, the owner's rotation was discarded entirely, so the local (1,0,0) offset
	// would never have been rotated and centerOfView would always have been (1,0,0)+(10,0,0),
	// giving a position of (11,0,5) instead. The rotated value below was confirmed empirically
	// (not re-derived by hand), matching this codebase's established decompose+conjugate
	// convention for "rotate a local axis into world space" (see LightFunctions.cpp).
	EXPECT_NEAR(camera.GetPosition().x, 10.f, 0.0001f);
	EXPECT_NEAR(camera.GetPosition().y, 0.f, 0.0001f);
	EXPECT_NEAR(camera.GetPosition().z, 6.f, 0.0001f);
}

} // namespace GLEngine::Renderer::Cameras
