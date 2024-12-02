#include <GUIStdafx.h>

#include <GUI/FileDialogWindow.h>
#include <GUI/GUIManager.h>
#include <GUI/GUIUtils.h>
#include <GUI/ReflectionGUI.h>

#include <Renderer/IRenderer.h>
#include <Renderer/Textures/TextureManager.h>
#include <Renderer/Textures/TextureResource.h>

#include <Core/Application.h>
#include <Core/Resources/ResourceHandle.h>
#include <Core/Resources/ResourceManager.h>

#include <filesystem>
#include <imgui.h>
#include <imgui_internal.h>

namespace GLEngine::GUI {

static C_GUIManager* s_GUIMgr = nullptr;

//=================================================================================
void DrawText(rttr::instance& obj, const rttr::property& prop)
{
	using namespace ::Utils::Reflection;

	::ImGui::Text((char*)prop.get_value(obj).get_wrapped_value<std::string>().c_str());
}

//=================================================================================
bool DrawVec3(rttr::instance& obj, const rttr::property& prop)
{
	using namespace ::Utils::Reflection;

	return ::ImGui::InputFloat3(GetMetadataMember<UI::Vec3::Name>(prop).c_str(), (float*)(&prop.get_value(obj).get_wrapped_value<glm::vec3>()));
}

//=================================================================================
bool DrawCheckbox(rttr::instance& obj, const rttr::property& prop)
{
	using namespace ::Utils::Reflection;

	return ::ImGui::Checkbox(GetMetadataMember<UI::Checkbox::Name>(prop).c_str(), (bool*)(&prop.get_value(obj).get_wrapped_value<bool>()));
}

//=================================================================================
bool DrawSlider(rttr::instance& obj, const rttr::property& prop)
{
	using namespace ::Utils::Reflection;

	return ::ImGui::SliderFloat(GetMetadataMember<UI::Slider::Name>(prop).c_str(), (float*)(&prop.get_value(obj).get_wrapped_value<float>()),
								GetMetadataMember<UI::Slider::Min>(prop), GetMetadataMember<UI::Slider::Max>(prop));
}

//=================================================================================
bool DrawSliderInt(rttr::instance& obj, const rttr::property& prop)
{
	using namespace ::Utils::Reflection;

	return ::ImGui::SliderInt(GetMetadataMember<UI::SliderInt::Name>(prop).c_str(), (prop.get_value(obj).convert<int*>()), GetMetadataMember<UI::SliderInt::Min>(prop),
							  GetMetadataMember<UI::SliderInt::Max>(prop));
}

//=================================================================================
bool DrawAngle(rttr::instance& obj, const rttr::property& prop)
{
	using namespace ::Utils::Reflection;

	return ::ImGui::SliderAngle(GetMetadataMember<UI::Angle::Name>(prop).c_str(), (float*)(&prop.get_value(obj).get_wrapped_value<float>()),
								GetMetadataMember<UI::Angle::Min>(prop), GetMetadataMember<UI::Angle::Max>(prop));
}

//=================================================================================
bool DrawColour(rttr::instance& obj, const rttr::property& prop)
{
	using namespace ::Utils::Reflection;

	return ::ImGui::ColorEdit3(GetMetadataMember<UI::Colour::Name>(prop).c_str(), (float*)(&prop.get_value(obj).get_wrapped_value<glm::vec3>()));
}

//=================================================================================
bool DrawTextureResource(rttr::instance& obj, const rttr::property& prop)
{
	using namespace ::Utils::Reflection;

	const static std::size_t maxStringLen = 30; // found out by experiment

	std::reference_wrapper<Core::ResourceHandle<Renderer::TextureResource>> resource
		= const_cast<Core::ResourceHandle<Renderer::TextureResource>&>(prop.get_value(obj).get_wrapped_value<Core::ResourceHandle<Renderer::TextureResource>>());
	const auto	   propertyName = GetMetadataMember<UI::Texture::Name>(prop);
	bool		   ret			= false;
	const ImVec2   drawAreaSz(std::min(380.f, ImGui::GetWindowWidth()), 88);
	const ImVec2   canvas_p0 = ImGui::GetCursorPos();
	const ImRect   imageRect(canvas_p0, canvas_p0 + drawAreaSz);
	const bool	   is_hovered = ImGui::IsItemHovered(); // Hovered
	const bool	   is_active  = ImGui::IsItemActive();	// Held
	ImDrawList*	   draw_list  = ImGui::GetWindowDrawList();
	const auto	   canvas_pos = ImGui::GetCursorScreenPos();
	const ImGuiIO& io		  = ImGui::GetIO();
	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{0, 0});

	{
		auto filename = resource.get().GetFilePath().generic_string();
		ImGui::BeginChildFrame(ImGuiID{static_cast<unsigned int>(std::hash<std::string>{}(propertyName + filename))}, drawAreaSz);

		ImGui::SetCursorPos(ImVec2{2, 2});
		{
			const ImVec2& previewSize{drawAreaSz.y - 4, drawAreaSz.y - 4};
			ImGui::BeginChildFrame(ImGuiID{static_cast<unsigned int>(std::hash<std::string>{}(propertyName + filename + "_preview"))}, previewSize);
			if (resource.get())
			{
				auto& tMGR			 = Core::C_Application::Get().GetActiveRenderer().GetTextureManager();
				auto  rendererHandle = tMGR.GetOrCreateTexture(resource);
				auto* GUIHandle		 = Core::C_Application::Get().GetActiveRenderer().GetTextureGUIHandle(rendererHandle);
				ImGui::Image((void*)(intptr_t)(GUIHandle), previewSize);
			}
			ImGui::EndChildFrame();
		}
		ImGui::SetCursorPos(ImVec2{drawAreaSz.y + 2, 6});
		ImGui::Text(propertyName.c_str());
		if (filename.size() > maxStringLen && filename.empty() == false)
		{
			filename.erase(0, filename.size() - maxStringLen + 3);
			filename = std::string("...") + filename;
		}
		else
		{
			ImGui::Text("Empty");
		}
		ImGui::SetCursorPos(ImVec2{drawAreaSz.y + 2, 30});
		ImGui::Text(filename.c_str());
		const std::string loadImageText{"Load image"};
		const ImGuiStyle& style			 = GImGui->Style;
		const auto		  buttonTextSize = ImGui::CalcTextSize(loadImageText.c_str(), nullptr, true);
		const auto		  buttonSize	 = ImVec2(buttonTextSize.x + style.FramePadding.x * 2.0f, buttonTextSize.y + style.FramePadding.y * 2.0f);
		ImGui::SetCursorPos(drawAreaSz - (buttonSize + ImVec2(14, 14)));
		ImGui::PopStyleVar(2);
		if (ImGui::Button(loadImageText.c_str()))
		{
			// TODO:
			//	[ ] Create generic file gui window

			auto&			   rm			= Core::C_ResourceManager::Instance();
			const auto		   resourceType = rttr::type::get<Renderer::TextureResource>();
			const auto		   x			= resourceType.get_method("GetResourceTypeHashStatic").invoke({}).get_value<std::size_t>();
			auto			   extensions	= rm.GetSupportedExtesnions(x);
			std::ostringstream oss;
			std::copy(extensions.begin(), extensions.end(), std::ostream_iterator<std::string>(oss, ", "));
			const std::string extensionsO = oss.str();

			const std::string extesnionsList(extensionsO.substr(0, extensionsO.size() - 2));
			const auto		  imageLoaderGUID	 = NextGUID();
			auto*			  resourcePathSelect = new GUI::C_FileDialogWindow(
				extensionsO, "Select image",
				[resource, imageLoaderGUID, prop, obj](const std::filesystem::path& path, GUI::C_GUIManager& guiMgr) mutable {
					// lets pass GUI MGR as argument
					auto& rm	   = Core::C_ResourceManager::Instance();
					resource.get() = rm.LoadResource<Renderer::TextureResource>(path);

					prop.set_value(obj, resource);
					guiMgr.DestroyWindow(imageLoaderGUID);
				},
				imageLoaderGUID, "./Images");
			s_GUIMgr->AddCustomWindow(resourcePathSelect);
			resourcePathSelect->SetVisible();
		}

		if (resource.get().IsReady())
		{
			if (DrawSquareButton(draw_list, canvas_pos + ImVec2(drawAreaSz.x, 0) - ImVec2(20, -4), E_ButtonType::Cross) && io.MouseReleased[0] && resource.get().IsReady())
			{
				ret			   = true;
				resource.get() = {};
			}
		}


		ImGui::EndChildFrame();
	}

	ImGui::SetCursorPos(canvas_p0);
	ImGui::ItemSize(imageRect);
	return false;
}

//=================================================================================
std::vector<rttr::property> DrawAllPropertyGUI(rttr::instance& obj)
{
	using namespace ::Utils::Reflection;
	std::vector<rttr::property> changedVals;

	if (UI::IsTypeUIMetaClass<MetaGUI::CustomGUIWidget>(obj))
	{
		// custom widgets
		GetMetadataMember<UI::CustomGUIWidget::DrawFunction>(obj)(obj);
		return {};
	}

	rttr::type type = obj.get_type().get_raw_type();
	if (type.get_derived_classes().empty() == false)
		type = obj.get_derived_type();

	std::map<std::string, std::vector<rttr::property>> collapsableHeaders;
	for (auto& prop : type.get_properties())
	{
		if (HasMetadataMember<MetaGUIInfo::CollapsableGroup>(prop))
		{
			collapsableHeaders[GetMetadataMember<MetaGUIInfo::CollapsableGroup>(prop)].emplace_back(prop);
		}
		else
		{
			if (DrawPropertyGUI(obj, prop))
			{
				changedVals.emplace_back(prop);
			}
		}
	}
	for (auto& collapsableHeader : collapsableHeaders)
	{
		if (::ImGui::CollapsingHeader(collapsableHeader.first.c_str()))
		{
			for (auto& prop : collapsableHeader.second)
			{
				if (DrawPropertyGUI(obj, prop))
				{
					changedVals.emplace_back(prop);
				}
			}
		}
	}
	return changedVals;
}

//=================================================================================
bool DrawPropertyGUI(rttr::instance& obj, const rttr::property& prop)
{
	using namespace ::Utils::Reflection;
	if (UI::IsUIMetaclass<MetaGUI::Vec3>(prop))
	{
		return DrawVec3(obj, prop);
	}
	else if (UI::IsUIMetaclass<MetaGUI::Checkbox>(prop))
	{
		return DrawCheckbox(obj, prop);
	}
	else if (UI::IsUIMetaclass<MetaGUI::Colour>(prop))
	{
		return DrawColour(obj, prop);
	}
	else if (UI::IsUIMetaclass<MetaGUI::Slider>(prop))
	{
		return DrawSlider(obj, prop);
	}
	else if (UI::IsUIMetaclass<MetaGUI::SliderInt>(prop))
	{
		return DrawSliderInt(obj, prop);
	}
	else if (UI::IsUIMetaclass<MetaGUI::Angle>(prop))
	{
		return DrawAngle(obj, prop);
	}
	else if (UI::IsUIMetaclass<MetaGUI::Text>(prop))
	{
		DrawText(obj, prop);
		return false;
	}
	else if (UI::IsUIMetaclass<MetaGUI::Texture>(prop))
	{
		return DrawTextureResource(obj, prop);
	}
	else
	{
		rttr::instance ins{prop.get_value(obj)};
		return GUI::DrawAllPropertyGUI(ins).empty() == false; // this will not return changed properties
	}
}

void SetGUIManager(C_GUIManager& mgr)
{
	s_GUIMgr = &mgr;
}

} // namespace GLEngine::GUI
