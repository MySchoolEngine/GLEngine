#pragma once

#include <GLRenderer/GLFW/GLFWoGLWindow.h>

#include <GLRenderer/Textures/Texture.h>
#include <GLRenderer/CameraManager.h>
#include <GLRenderer/GUI/PlotLine.h>
#include <GLRenderer/GUI/CheckBoxValue.h>

#include <Entity/World.h>

#include <Core/EventSystem/LayerStack.h>

#include <Utils/HighResolutionTimer.h>

namespace GLEngine {

namespace Core {
class C_AppEvent;
}

namespace Renderer {
class I_CameraComponent;
}

namespace GLRenderer {

namespace ImGui {
class C_ImGuiLayer;
}

namespace Mesh {
class C_StaticMeshResource;
}

namespace Buffers {
namespace UBO {
class C_FrameConstantsBuffer;
}
}

class C_TerrainEntity;

namespace Windows {
class C_ExplerimentWindow : public GLFW::C_GLFWoGLWindow {
	using T_Base = GLFW::C_GLFWoGLWindow;
public:
	C_ExplerimentWindow(const Core::S_WindowInfo& wndInfo);
	virtual ~C_ExplerimentWindow() = default;
	//=================================================================================
	virtual void Update() override;

	//=================================================================================
	virtual void OnEvent(Core::I_Event& event) override;
	virtual void Init(const Core::S_WindowInfo& wndInfo) override;

protected:
	bool OnKeyPressed(Core::C_KeyPressedEvent& event);
	bool OnAppInit(Core::C_AppEvent& event);

private:
	void SetupWorld();
	void MouseSelect();

	void sampleTime(double new_sample);

	void SetupNoiseTex();

	std::shared_ptr<Renderer::I_CameraComponent> GetCameraComponent() const;

	Entity::C_World												m_World;
	std::weak_ptr<Entity::I_Entity>								m_Player;
	std::shared_ptr<Buffers::UBO::C_FrameConstantsBuffer>		m_FrameConstUBO;
	std::shared_ptr<C_TerrainEntity>							m_Terrain;
	Textures::C_Texture											m_texture;
	Core::C_LayerStack											m_LayerStack;
	Temporar::C_CameraManager									m_CamManager;
	ImGui::C_ImGuiLayer*										m_ImGUI;
	Utils::HighResolutionTimer									m_FrameTimer;
	GUI::C_PlotLine<500>										m_Samples;
	GUI::C_CheckBoxValue											m_VSync;
};

}}}