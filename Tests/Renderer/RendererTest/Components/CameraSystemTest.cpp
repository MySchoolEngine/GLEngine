#include <RendererTestStdafx.h>

#include <Renderer/Cameras/OrbitalCamera.h>
#include <Renderer/Components/CameraComponent.h>
#include <Renderer/ICamera.h>
#include <Renderer/Systems/CameraSystem.h>

#include <Physics/Primitives/Frustum.h>

#include <Entity/ComponentRegistry.h>
#include <Entity/Components/CoreComponents.h>
#include <Entity/Entity.h>
#include <Entity/Systems/TransformSystem.h>
#include <Entity/World.h>

namespace GLEngine::Renderer {

namespace {

// Records what CameraSystem::Update feeds it, without implementing any real camera math.
class C_FakeCamera final : public I_Camera {
public:
	C_FakeCamera()
		: I_Camera(nullptr)
	{
	}

	void SetOwnerWorldTransform(const glm::mat4& worldTransform) override
	{
		lastWorldTransform = worldTransform;
		++setOwnerWorldTransformCalls;
	}
	void Update() override { ++updateCalls; }

	glm::mat4 GetViewProjectionMatrix() const override { return glm::mat4(1.f); }
	glm::mat4 GetScreenToWorldMatrix() const override { return glm::mat4(1.f); }
	glm::mat4 GetProjectionMatrix() const override { return glm::mat4(1.f); }
	glm::mat4 GetViewMatrix() const override { return glm::mat4(1.f); }
	glm::quat GetRotation() const override { return glm::quat(1.f, 0.f, 0.f, 0.f); }
	glm::vec3 GetDirection() const override { return glm::vec3(0.f, 0.f, -1.f); }
	glm::vec3 GetPosition() const override { return glm::vec3(0.f); }
	float	  GetFar() const override { return 100.f; }
	float	  GetNear() const override { return 0.1f; }

	Physics::Primitives::C_Frustum GetFrustum() const override
	{
		return Physics::Primitives::C_Frustum(GetPosition(), glm::vec3(0.f, 1.f, 0.f), GetDirection(), GetNear(), GetFar(), 1.f, 1.f);
	}

	bool			 HasDebugDrawGUI() const override { return false; }
	std::string_view GetDebugComponentName() const override { return "FakeCamera"; }

	glm::mat4 lastWorldTransform		  = glm::mat4(1.f);
	int		  setOwnerWorldTransformCalls = 0;
	int		  updateCalls				  = 0;
};

} // namespace

TEST(CameraSystem, Update_FeedsOwnerWorldTransformBeforeUpdating)
{
	Entity::C_World world;
	auto			entity								   = world.CreateEntity("Camera");
	entity.Get<Entity::S_TransformComponent>().translation = glm::vec3(3.f, 4.f, 5.f);

	auto fakeCamera						   = std::make_shared<C_FakeCamera>();
	entity.Add<S_CameraComponent>().camera = fakeCamera;

	Entity::TransformSystem::Update(world);
	CameraSystem::Update(world);

	ASSERT_EQ(fakeCamera->setOwnerWorldTransformCalls, 1);
	EXPECT_EQ(fakeCamera->updateCalls, 1);
	EXPECT_EQ(glm::vec3(fakeCamera->lastWorldTransform[3]), glm::vec3(3.f, 4.f, 5.f));
}

TEST(CameraSystem, OrbitalCamera_EffectivePositionIncludesParentTranslation)
{
	Entity::C_World world;
	auto			parent								   = world.CreateEntity("Parent");
	auto			cameraEntity						   = world.CreateEntity("Camera");
	parent.Get<Entity::S_TransformComponent>().translation = glm::vec3(10.f, 0.f, 0.f);
	ASSERT_TRUE(world.SetParent(cameraEntity.Handle(), parent.Handle()));

	auto camera = std::make_shared<Cameras::C_OrbitalCamera>();
	camera->SetupCameraProjection(0.1f, 100.f, 1.f, 90.f);
	camera->SetupCameraView(/*zoom=*/5.f, /*center=*/glm::vec3(0.f), /*angleXDeg=*/90.f, /*angleYDeg=*/0.f);

	cameraEntity.Add<S_CameraComponent>().camera = camera;

	Entity::TransformSystem::Update(world);
	CameraSystem::Update(world);

	const auto pos = camera->GetPosition();
	EXPECT_NEAR(pos.x, 10.f, 0.0001f);
	EXPECT_NEAR(pos.y, 0.f, 0.0001f);
	EXPECT_NEAR(pos.z, 5.f, 0.0001f);
}

TEST(CameraComponents, Camera_IsRegisteredInComponentRegistry)
{
	EnsureCameraComponentsRegistered();

	EXPECT_NE(Entity::C_ComponentRegistry::Instance().FindByName("Camera"), nullptr);
}

} // namespace GLEngine::Renderer
