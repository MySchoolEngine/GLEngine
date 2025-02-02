#pragma once

#include <GUI/GUIPart.h>

#include <Renderer/Resources/RenderResourceHandle.h>


struct ImRect;
namespace GLEngine::GUI {
class GUI_API_EXPORT C_ImageViewer : public I_GUIPart {
public:
	explicit C_ImageViewer(Renderer::Handle<Renderer::Texture> texture);
	bool Draw() const override;

	void SetSize(const glm::vec2 dim);
	void SetBackground(Renderer::Handle<Renderer::Texture> texture, const glm::vec2& scale = glm::vec2{1.f, 1.f});
	void AddOverlay(Renderer::Handle<Renderer::Texture> texture, bool flipVertically = false);
	void PopOverlay();

private:
	struct RenderSettings {
		Renderer::Handle<Renderer::Texture> Handle;
		bool								bFlipVertically = false;
		glm::vec2							Scale;
	};
	RenderSettings						m_Background;
	Renderer::Handle<Renderer::Texture> m_Texture;
	glm::vec2							m_Size; //< Size of GUI, not the image
	mutable float						m_Zoom;
	mutable void*						m_GUIHandle;
	std::vector<RenderSettings>			m_Overlays;

	static void DrawImageWithCorrectAxis(void* const imageGPUHandle, const ImVec2& drawArea, const ImRect& rect, bool flipVertically);
};
} // namespace GLEngine::GUI
