#pragma once

#include <GLRenderer/GLFW/GLFWoGLWindow.h>

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
	std::shared_ptr<Mesh::C_StaticMeshResource>					m_Mesh;
	std::shared_ptr<Buffers::UBO::C_FrameConstantsBuffer>		m_FrameConstUBO;
	std::shared_ptr<Cameras::C_OrbitalCamera>					m_OrbitalCamera;

};
}
}
}