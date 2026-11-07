#include <EditorStdafx.h>

#include <Renderer/IRenderer.h>
#include <Renderer/Materials/MaterialResource.h>
#include <Renderer/Mesh/Loading/MeshResource.h>
#include <Renderer/Textures/TextureManager.h>
#include <Renderer/Textures/TextureResource.h>

#include <GUI/GUIManager.h>
#include <GUI/GUIUtils.h>
#include <GUI/ReflectionGUI.h>
#include <GUI/ResourceDialogWindow.h>

#include <Core/Application.h>
#include <Core/GUID.h>
#include <Core/Resources/ResourceHandle.h>
#include <Core/Resources/ResourceManager.h>

#include <Utils/Serialization/SerializationTraits.h>

#include <cstdio>
#include <filesystem>
#include <imgui.h>
#include <imgui_internal.h>
#include <optional>

// GUI must not link Renderer (Renderer already links GUI; the reverse would be a project-
// reference cycle - see CLAUDE.md), but drawing a resource-handle value needs concrete Renderer
// types. Renderer tags its own resource handle types with RegisterMetaclass<MetaGUI::
// CustomGUIWidget>() at their own original registration (see Resource.h's DECLARE_RESOURCE_TYPE
// and MaterialResource.cpp) - that's just a marker, no function pointer, since RTTR does not
// merge type-level metadata attached from a second class_<T>() call once a type is already fully
// registered elsewhere (only members/methods merge that way). Editor, which links both GUI and
// Renderer, instead registers the actual drawer into GUI's plain, non-RTTR registry at its own
// static init below.
namespace GLEngine::Editor { namespace {

//=================================================================================
template <Core::IsResource resourceType, class MetaClassEnum> bool DrawResource(rttr::instance& obj, const rttr::property* prop)
{
	using namespace ::Utils::Reflection;
	using namespace ::GLEngine::GUI;

	// obj is already the resource handle value itself (GUI unwrapped it before dispatching here),
	// never the owning object - try_convert gives a pointer into the real, mutable storage.
	auto* resourcePtr = obj.try_convert<Core::ResourceHandle<resourceType>>();
	GLE_ASSERT(resourcePtr != nullptr, "CustomWidgetDrawFunc called with the wrong instance type");

	static constexpr std::size_t s_MaxStringLen = 30; // found out by experiment

	std::reference_wrapper resource(*resourcePtr);
	bool				   ret = false;
	const ImVec2		   drawAreaSz(std::min(380.f, ImGui::GetWindowWidth()), 88);
	const ImVec2		   canvasP0	 = ImGui::GetCursorPos();
	const auto			   canvasPos = ImGui::GetCursorScreenPos();
	const ImRect		   imageRect(canvasPos, canvasPos + drawAreaSz);
	const bool			   isHovered = ImGui::IsItemHovered(); // Hovered
	const bool			   isActive	 = ImGui::IsItemActive();  // Held
	ImDrawList*			   drawList	 = ImGui::GetWindowDrawList();
	const ImGuiIO&		   io		 = ImGui::GetIO();
	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{0, 0});

	{
		// prop is null only for the hypothetical "resource handle is the true root object" case
		// (never actually reached in practice - resource handles are always nested inside a
		// component/material); the type's own rttr name is the sane fallback label there.
		const std::string label	   = prop ? GetMetadataMember<MetaClassEnum::Name>(*prop) : rttr::type::get<resourceType>().get_name().to_string();
		auto			  filename = resource.get().GetFilePath().generic_string();
		ImGuiID			  FrameID  = ImGuiID{static_cast<unsigned int>(std::hash<std::string>{}(label + filename))};
		ImGui::BeginChildFrame(FrameID, drawAreaSz);

		auto&	   resMgr = Core::C_ResourceManager::Instance();
		const auto loader = resMgr.GetLoaderForType<resourceType>();

		if (loader.has_value() && ImGui::BeginDragDropTargetCustom(imageRect, FrameID))
		{
			const auto dragDropLabel = loader.transform([](const auto& l) { return l.get().DragAndDropLabel(); }).value();
			if (ImGui::AcceptDragDropPayload(dragDropLabel.c_str(), ImGuiDragDropFlags_AcceptPeekOnly))
			{
				ImGui::GetForegroundDrawList()->AddRectFilled(imageRect.Min, imageRect.Max, IM_COL32(0, 255, 0, 40));
				ImGui::GetForegroundDrawList()->AddRect(imageRect.Min, imageRect.Max, IM_COL32(0, 255, 0, 255), 0.0f, 0, 2.0f);
			}
			if (auto* p = ImGui::AcceptDragDropPayload(dragDropLabel.c_str()))
			{
				const std::filesystem::path droppedPath(static_cast<const char*>(p->Data));
				resource.get() = resMgr.LoadResource<resourceType>(droppedPath);
				ret			   = true;
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::SetCursorPos(ImVec2{2, 2});
		{
			const ImVec2 previewSize{drawAreaSz.y - 4, drawAreaSz.y - 4};
			ImGui::BeginChildFrame(ImGuiID{static_cast<unsigned int>(std::hash<std::string>{}(label + filename + "_preview"))}, previewSize);
			if constexpr (std::is_same_v<resourceType, Renderer::TextureResource>)
			{
				if (resource.get())
				{
					auto& tMGR			 = Core::C_Application::Get().GetActiveRenderer().GetTextureManager();
					auto  rendererHandle = tMGR.GetOrCreateTexture(resource);
					auto* GUIHandle		 = Core::C_Application::Get().GetActiveRenderer().GetTextureGUIHandle(rendererHandle);
					ImGui::Image((void*)(intptr_t)(GUIHandle), previewSize);
				}
			}
			ImGui::EndChildFrame();
		}
		ImGui::SetCursorPos(ImVec2{drawAreaSz.y + 2, 6});
		ImGui::TextUnformatted(label.c_str());
		if (filename.size() > s_MaxStringLen && filename.empty() == false)
		{
			filename.erase(0, filename.size() - s_MaxStringLen + 3);
			filename = std::string("...") + filename;
		}
		else
		{
			ImGui::TextUnformatted("Empty");
		}
		ImGui::SetCursorPos(ImVec2{drawAreaSz.y + 2, 30});
		ImGui::TextUnformatted(filename.c_str());
		std::string loadImageText;
		if constexpr (std::is_same_v<resourceType, Renderer::TextureResource>)
		{
			loadImageText = "Load image";
		}
		else if constexpr (std::is_same_v<resourceType, Renderer::MeshResource>)
		{
			loadImageText = "Load model";
		}
		else
		{
			// Fallback for any resource type not special-cased above
			loadImageText = "Load";
		}

		const ImGuiStyle& style			 = GImGui->Style;
		const auto		  buttonTextSize = ImGui::CalcTextSize(loadImageText.c_str(), nullptr, true);
		const auto		  buttonSize	 = ImVec2(buttonTextSize.x + style.FramePadding.x * 2.0f, buttonTextSize.y + style.FramePadding.y * 2.0f);
		ImGui::SetCursorPos(drawAreaSz - (buttonSize + ImVec2(14, 14)));
		ImGui::PopStyleVar(2);
		if (ImGui::Button(loadImageText.c_str()))
		{
			// TODO:
			//	[ ] Create generic file gui window
			const auto imageLoaderGUID	  = NextGUID();
			auto*	   resourcePathSelect = new C_ResourceDialogWindow(resource, "Select image", imageLoaderGUID);

			GetGUIManager()->AddCustomWindow(resourcePathSelect);
			resourcePathSelect->SetVisible();
		}

		if (resource.get().IsReady())
		{
			if (DrawSquareButton(drawList, canvasPos + ImVec2(drawAreaSz.x, 0) - ImVec2(20, -4), E_ButtonType::Cross) && io.MouseReleased[0] && resource.get().IsReady())
			{
				ret			   = true;
				resource.get() = {};
			}
		}


		ImGui::EndChildFrame();
	}

	ImGui::SetCursorPos(canvasP0);
	ImGui::ItemSize(imageRect);
	return ret;
}

//=================================================================================
struct S_ResourcePropertyDrawerRegistration {
	S_ResourcePropertyDrawerRegistration()
	{
		using namespace ::Utils::Reflection;
		using namespace ::GLEngine::GUI;
		RegisterCustomWidgetDrawer(rttr::type::get<Core::ResourceHandle<Renderer::TextureResource>>(), &DrawResource<Renderer::TextureResource, UI::Texture>);
		RegisterCustomWidgetDrawer(rttr::type::get<Core::ResourceHandle<Renderer::MeshResource>>(), &DrawResource<Renderer::MeshResource, UI::MeshResource>);
		RegisterCustomWidgetDrawer(rttr::type::get<Core::ResourceHandle<Renderer::MaterialResource>>(), &DrawResource<Renderer::MaterialResource, UI::MaterialResource>);
	}
} s_ResourcePropertyDrawerRegistration;

}} // namespace GLEngine::Editor
