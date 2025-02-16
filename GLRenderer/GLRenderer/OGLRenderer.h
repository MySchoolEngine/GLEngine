#pragma once

#include <GLRenderer/GLResourceManager.h>

#include <Renderer/IRenderer.h>

#include <GUI/Input/Button.h>
#include <GUI/Input/CheckBoxValue.h>
#include <GUI/Menu/Menu.h>
#include <GUI/PlotLine.h>
#include <GUI/Text.h>

#include <Core/CoreMacros.h>

namespace GLEngine {

namespace GUI::Menu {
class C_MenuItem;
}

namespace GLRenderer {
class C_GLDevice;

class C_OGLRenderer : public Renderer::I_Renderer {
public:
	C_OGLRenderer(C_GLDevice&);
	~C_OGLRenderer() override;

	//=================================================================================
	// Renderer::I_Renderer
	//=================================================================================
	void AddCommand(Renderer::I_Renderer::T_CommandPtr) override;
	void AddTransferCommand(T_CommandPtr) override; //< obsolete

	void Commit() const override;
	void ClearCommandBuffers() override;


	//=================================================================================
	void Lock(bool lock = true) override;

	GUID SetupControls(GUI::C_GUIManager& guiMan);
	void DestroyControls(GUI::C_GUIManager& guiMan);

	//=================================================================================
	Renderer::E_PassType GetCurrentPassType() const override;
	void				 SetCurrentPassType(Renderer::E_PassType type) override;

	bool				WantWireframe() const { return m_Wireframe.GetValue(); }
	Renderer::I_Device& GetDevice() override;
	void				SetBufferData(Renderer::Handle<Renderer::Buffer> dstBuffer, std::size_t numBytes, const void* data) override;
	void				SetTextureData(Renderer::Handle<Renderer::Texture> dstTexture, const Renderer::I_TextureViewStorage& storage) override;
	void				SetTextureSampler(Renderer::Handle<Renderer::Texture> dstTexture, Renderer::Handle<Renderer::Sampler> srcSampler) override;
	void*				GetTextureGUIHandle(Renderer::Handle<Renderer::Texture> texture) override;

	Renderer::ResourceManager& GetRM() override;
	GLResourceManager&		   GetRMGR();

private:
	void CaptureCommands() const;

	bool											 m_Locked = false;
	std::vector<Renderer::I_Renderer::T_CommandPtr>* m_CommandQueue;
	std::vector<Renderer::I_Renderer::T_CommandPtr>	 m_TransferQueue;
	mutable std::mutex								 m_CommandQueueMTX;
	mutable std::mutex								 m_TransferQueueMTX;
	GUI::C_PlotLine<500>							 m_DrawCommands;
	GUI::Input::C_CheckBoxValue						 m_CatchErrors;
	GUI::Input::C_CheckBoxValue						 m_Wireframe;
	GUI::Input::C_Button							 m_ScreenCaptureList;
	bool											 m_OutputCommandList = false;
	bool											 m_PreviousCatchErrorsVal;
	Renderer::E_PassType							 m_CurrentPass;

	GLResourceManager m_GPUResourceManager;

	C_GLDevice& m_Device;

	enum class E_GUITexts : std::uint8_t {
		AvgDrawCommands,
		MinMax,
		DrawCalls,
		UBOMemoryUsage,
		Last,
	};
	std::array<GUI::C_FormatedText, static_cast<int>(E_GUITexts::Last)> m_GUITexts;
	GUID																m_Window;
	GUI::Menu::C_Menu													m_Windows;
};

} // namespace GLRenderer
} // namespace GLEngine