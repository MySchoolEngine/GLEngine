#pragma once

#include <GLRenderer/GLFW/GLFWoGLWindow.h>

#include <Entity/World.h>

namespace GLEngine {
namespace GLRenderer {

namespace Cameras {
class C_OrbitalCamera;
}

namespace Mesh {
class C_StaticMeshResource;
}

namespace Buffers {
namespace UBO {
class C_FrameConstantsBuffer;
}
}

namespace Windows {
class C_ExplerimentWindow : public GLFW::C_GLFWoGLWindow {
public:
	C_ExplerimentWindow(const Core::S_WindowInfo& wndInfo);
	//=================================================================================
	virtual void Update() override;
private:
	void SetupWorld(const Core::S_WindowInfo& wndInfo);

	Entity::C_World												m_World;
	std::weak_ptr<Entity::I_Entity>								m_Player;
	std::shared_ptr<Buffers::UBO::C_FrameConstantsBuffer>		m_FrameConstUBO;

};
}
}
}