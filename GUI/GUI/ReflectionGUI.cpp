#include <GUIStdafx.h>

#include <GUI/FileDialogWindow.h>
#include <GUI/GUIManager.h>
#include <GUI/GUIUtils.h>
#include <GUI/ReflectionGUI.h>
#include <GUI/ResourceDialogWindow.h>

#include <Renderer/IRenderer.h>
#include <Renderer/Materials/MaterialResource.h>
#include <Renderer/Mesh/Loading/MeshResource.h>
#include <Renderer/Textures/TextureManager.h>
#include <Renderer/Textures/TextureResource.h>

#include <Core/Application.h>
#include <Core/Resources/ResourceHandle.h>
#include <Core/Resources/ResourceManager.h>

#include <Utils/Serialization/SerializationTraits.h>

#include <cstdio>
#include <filesystem>
#include <imgui.h>
#include <imgui_internal.h>
#include <optional>

namespace GLEngine::GUI {
namespace {

static C_GUIManager* s_GUIMgr = nullptr;

//=================================================================================
// Edits a wrapped value in place, or copies out/edits/reassigns a plain one.
template <class T, class Func> bool EditValue(rttr::variant& value, Func&& func)
{
	if (value.get_type().is_wrapper())
	{
		// get_wrapped_value() returns const T&, but the underlying storage is mutable here.
		return func(const_cast<T&>(value.get_wrapped_value<T>()));
	}
	T local = value.get_value<T>();
	if (func(local) == false)
		return false;
	value = local;
	return true;
}

//=================================================================================
// obj may own `prop` (property-level call) or be the value itself (array element);
// try_convert first so the latter case doesn't hit an invalid get_value() result.
template <class T> rttr::variant GetPropertyOrSelfValue(rttr::instance obj, const rttr::property& prop)
{
	if (auto* directPtr = obj.try_convert<T>())
		return rttr::variant(std::ref(*directPtr));
	return prop.get_value(obj);
}

//=================================================================================
void DrawText(rttr::instance obj, const rttr::property& prop)
{
	auto value = GetPropertyOrSelfValue<std::string>(obj, prop);
	const std::string text = value.get_type().is_wrapper() ? value.get_wrapped_value<std::string>() : value.get_value<std::string>();
	::ImGui::TextUnformatted(text.c_str());
}

//=================================================================================
bool DrawVec3(rttr::instance obj, const rttr::property& prop)
{
	using namespace ::Utils::Reflection;

	auto value = GetPropertyOrSelfValue<glm::vec3>(obj, prop);
	return EditValue<glm::vec3>(value, [&](glm::vec3& v) { return ::ImGui::InputFloat3(GetMetadataMember<UI::Vec3::Name>(prop).c_str(), (float*)&v); });
}

//=================================================================================
bool DrawiVec2(rttr::instance obj, const rttr::property& prop)
{
	using namespace ::Utils::Reflection;

	auto value = GetPropertyOrSelfValue<glm::ivec2>(obj, prop);
	return EditValue<glm::ivec2>(value, [&](glm::ivec2& v) { return ::ImGui::InputInt2(GetMetadataMember<UI::iVec2::Name>(prop).c_str(), (int*)&v); });
}

//=================================================================================
bool DrawCheckbox(rttr::instance obj, const rttr::property& prop)
{
	using namespace ::Utils::Reflection;

	auto value = GetPropertyOrSelfValue<bool>(obj, prop);
	return EditValue<bool>(value, [&](bool& v) { return ::ImGui::Checkbox(GetMetadataMember<UI::Checkbox::Name>(prop).c_str(), &v); });
}

//=================================================================================
bool DrawSlider(rttr::instance obj, const rttr::property& prop)
{
	using namespace ::Utils::Reflection;

	auto value = GetPropertyOrSelfValue<float>(obj, prop);
	return EditValue<float>(value, [&](float& v) {
		return ::ImGui::SliderFloat(GetMetadataMember<UI::Slider::Name>(prop).c_str(), &v, GetMetadataMember<UI::Slider::Min>(prop), GetMetadataMember<UI::Slider::Max>(prop));
	});
}

//=================================================================================
// Takes the value directly, not rttr::instance: the enum's concrete type isn't
// known at compile time, so callers (including array elements) pass it as-is.
bool DrawEnumSelectValue(rttr::variant& value, const std::string& label)
{
	const bool isWrapped	= value.get_type().is_wrapper();
	const auto propertyType = isWrapped ? value.get_type().get_wrapped_type() : value.get_type();
	const auto enumeration	= propertyType.get_enumeration();
	GLE_ASSERT(enumeration.is_valid(), "Enumeration {} not registered", propertyType);

	bool	   changed	   = false;
	const auto currentName = isWrapped ? enumeration.value_to_name(value.extract_wrapped_value()) : enumeration.value_to_name(value);
	if (::ImGui::BeginCombo(label.c_str(), currentName.data()))
	{
		for (const auto& candidate : enumeration.get_values())
		{
			const bool isSelected = (value == candidate);
			if (::ImGui::Selectable(enumeration.value_to_name(candidate).data(), isSelected))
			{
				changed = true;
				if (isWrapped)
					(int&)value.get_wrapped_value<int>() = candidate.get_wrapped_value<int>();
				else
					value = candidate;
			}
			if (isSelected)
			{
				::ImGui::SetItemDefaultFocus();
			}
		}
		::ImGui::EndCombo();
	}
	return changed;
}

//=================================================================================
bool DrawEnumSelect(rttr::instance obj, const rttr::property& prop)
{
	using namespace ::Utils::Reflection;

	auto value = prop.get_value(obj);
	return DrawEnumSelectValue(value, GetMetadataMember<UI::EnumSelect::Name>(prop));
}

//=================================================================================
bool DrawEnumSelectOptional(rttr::instance obj, const rttr::property& prop)
{
	using namespace ::Utils::Reflection;

	auto value = prop.get_value(obj);

	auto& currentValRef = (std::optional<int>&)value.get_wrapped_value<std::optional<int>>();

	bool changed = false;

	const auto enumeration = value.get_type().get_wrapped_type().get_wrapped_type().get_raw_type().get_enumeration();
	GLE_ASSERT(enumeration.is_valid(), "Enumeration {} not registered", value.get_type().get_wrapped_type());
	if (::ImGui::BeginCombo(GetMetadataMember<UI::EnumSelectOptional::Name>(prop).c_str(),
							currentValRef.has_value() ? enumeration.value_to_name(currentValRef.value()).data() : GetMetadataMember<UI::EnumSelectOptional::OptionalName>(prop).c_str()))
	{
		for (const auto& candidate : enumeration.get_values())
		{
			const bool isSelected = (value == candidate); // todo wrong
			if (::ImGui::Selectable(enumeration.value_to_name(candidate).data(), isSelected))
			{
				changed		  = true;
				currentValRef = candidate.get_wrapped_value<int>();
			}
			if (isSelected)
			{
				::ImGui::SetItemDefaultFocus();
			}
		}
		if (::ImGui::Selectable(GetMetadataMember<UI::EnumSelectOptional::OptionalName>(prop).c_str(), !currentValRef.has_value()))
		{
			changed = true;
			currentValRef.reset();
		}
		if (currentValRef.has_value() == false)
		{
			::ImGui::SetItemDefaultFocus();
		}
		::ImGui::EndCombo();
	}
	return changed;
}

//=================================================================================
bool DrawSliderInt(rttr::instance obj, const rttr::property& prop)
{
	using namespace ::Utils::Reflection;

	auto value = GetPropertyOrSelfValue<int>(obj, prop);
	return EditValue<int>(value, [&](int& v) {
		return ::ImGui::SliderInt(GetMetadataMember<UI::SliderInt::Name>(prop).c_str(), &v, GetMetadataMember<UI::SliderInt::Min>(prop), GetMetadataMember<UI::SliderInt::Max>(prop));
	});
}

//=================================================================================
bool DrawAngle(rttr::instance obj, const rttr::property& prop)
{
	using namespace ::Utils::Reflection;

	auto value = GetPropertyOrSelfValue<float>(obj, prop);
	return EditValue<float>(value, [&](float& v) {
		return ::ImGui::SliderAngle(GetMetadataMember<UI::Angle::Name>(prop).c_str(), &v, GetMetadataMember<UI::Angle::Min>(prop), GetMetadataMember<UI::Angle::Max>(prop));
	});
}

//=================================================================================
bool DrawColour(rttr::instance obj, const rttr::property& prop)
{
	using namespace ::Utils::Reflection;

	auto value = GetPropertyOrSelfValue<glm::vec3>(obj, prop);
	return EditValue<glm::vec3>(value, [&](glm::vec3& v) { return ::ImGui::ColorEdit3(GetMetadataMember<UI::Colour::Name>(prop).c_str(), (float*)&v); });
}

//=================================================================================
template <Core::IsResource resourceType, class MetaClassEnum> bool DrawResource(rttr::instance obj, const rttr::property& prop)
{
	using namespace ::Utils::Reflection;

	auto value = GetPropertyOrSelfValue<Core::ResourceHandle<resourceType>>(obj, prop);

	static constexpr std::size_t s_MaxStringLen = 30; // found out by experiment

	std::reference_wrapper resource = const_cast<Core::ResourceHandle<resourceType>&>(value.template get_wrapped_value<Core::ResourceHandle<resourceType>>());
	bool				   ret		= false;
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
		auto	filename = resource.get().GetFilePath().generic_string();
		ImGuiID FrameID	 = ImGuiID{static_cast<unsigned int>(std::hash<std::string>{}(GetMetadataMember<MetaClassEnum::Name>(prop) + filename))};
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
			ImGui::BeginChildFrame(ImGuiID{static_cast<unsigned int>(std::hash<std::string>{}(GetMetadataMember<MetaClassEnum::Name>(prop) + filename + "_preview"))}, previewSize);
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
		ImGui::TextUnformatted(GetMetadataMember<MetaClassEnum::Name>(prop).c_str());
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

			s_GUIMgr->AddCustomWindow(resourcePathSelect);
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
// Element-level twin of DrawPropertyGUI: tag lives on the container property, but edits the element directly.
bool DrawArrayElementGUI(const rttr::property& prop, rttr::variant& elementVar)
{
	using namespace ::Utils::Reflection;

	if (UI::IsUIMetaclassForElement<MetaGUI::Vec3>(prop, elementVar))
	{
		return DrawVec3(elementVar, prop);
	}
	else if (UI::IsUIMetaclassForElement<MetaGUI::iVec2>(prop, elementVar))
	{
		return DrawiVec2(elementVar, prop);
	}
	else if (UI::IsUIMetaclassForElement<MetaGUI::Checkbox>(prop, elementVar))
	{
		return DrawCheckbox(elementVar, prop);
	}
	else if (UI::IsUIMetaclassForElement<MetaGUI::Colour>(prop, elementVar))
	{
		return DrawColour(elementVar, prop);
	}
	else if (UI::IsUIMetaclassForElement<MetaGUI::Slider>(prop, elementVar))
	{
		return DrawSlider(elementVar, prop);
	}
	else if (UI::IsUIMetaclassForElement<MetaGUI::SliderInt>(prop, elementVar))
	{
		return DrawSliderInt(elementVar, prop);
	}
	else if (UI::IsUIMetaclassForElement<MetaGUI::Angle>(prop, elementVar))
	{
		return DrawAngle(elementVar, prop);
	}
	else if (UI::IsUIMetaclassForElement<MetaGUI::Text>(prop, elementVar))
	{
		DrawText(elementVar, prop);
		return false;
	}
	else if (UI::IsUIMetaclassForElement<MetaGUI::EnumSelect>(prop, elementVar))
	{
		return DrawEnumSelectValue(elementVar, GetMetadataMember<UI::EnumSelect::Name>(prop));
	}
	else if (UI::IsUIMetaclassForElement<MetaGUI::Texture>(prop, elementVar))
	{
		return DrawResource<Renderer::TextureResource, UI::Texture>(elementVar, prop);
	}
	else if (UI::IsUIMetaclassForElement<MetaGUI::MeshResource>(prop, elementVar))
	{
		return DrawResource<Renderer::MeshResource, UI::MeshResource>(elementVar, prop);
	}
	else if (UI::IsUIMetaclassForElement<MetaGUI::MaterialResource>(prop, elementVar))
	{
		return DrawResource<Renderer::MaterialResource, UI::MaterialResource>(elementVar, prop);
	}
	return false;
}

//=================================================================================
bool DrawSequentialContainer(rttr::instance obj, const rttr::property& prop)
{
	using namespace ::Utils::Reflection;

	auto var  = prop.get_value(obj);
	auto view = var.create_sequential_view();

	const std::string label	   = HasMetadataMember<UI::Array::Name>(prop) ? GetMetadataMember<UI::Array::Name>(prop) : prop.get_name().to_string();
	const bool		  allowAdd = HasMetadataMember<UI::Array::AllowAdd>(prop) && GetMetadataMember<UI::Array::AllowAdd>(prop);

	bool changed = false;
	if (::ImGui::CollapsingHeader(label.c_str()))
	{
		std::optional<std::size_t> removeIndex;
		for (std::size_t i = 0; i < view.get_size(); ++i)
		{
			::ImGui::PushID(static_cast<int>(i));
			::ImGui::TextUnformatted(std::to_string(i).c_str());
			::ImGui::SameLine();

			rttr::variant elementVar = view.get_value(i);
			if (DrawArrayElementGUI(prop, elementVar))
			{
				view.set_value(i, elementVar);
				changed = true;
			}

			::ImGui::SameLine();
			if (::ImGui::SmallButton("-"))
				removeIndex = i;
			::ImGui::PopID();
		}
		if (removeIndex.has_value())
		{
			view.erase(view.begin() + static_cast<int>(*removeIndex));
			changed = true;
		}
		if (allowAdd && ::ImGui::Button("+ Add"))
		{
			view.insert(view.end(), view.get_value_type().create());
			changed = true;
		}
	}

	if (changed)
		prop.set_value(obj, var);
	return changed;
}
} // namespace

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
bool DrawPropertyGUI(rttr::instance obj, const rttr::property& prop)
{
	using namespace ::Utils::Reflection;
	const auto propType = prop.get_type().get_raw_type();
	// Check container-ness before any tag check: a vector may carry a Slider/Vec3/etc. tag for its elements.
	const bool isArray = propType.is_sequential_container() || (propType.is_wrapper() && propType.get_wrapped_type().get_raw_type().is_sequential_container());
	if (isArray)
	{
		return DrawSequentialContainer(obj, prop);
	}
	else if (UI::IsUIMetaclass<MetaGUI::Vec3>(prop))
	{
		return DrawVec3(obj, prop);
	}
	else if (UI::IsUIMetaclass<MetaGUI::iVec2>(prop))
	{
		return DrawiVec2(obj, prop);
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
		return DrawResource<Renderer::TextureResource, UI::Texture>(obj, prop);
	}
	else if (UI::IsUIMetaclass<MetaGUI::MeshResource>(prop))
	{
		return DrawResource<Renderer::MeshResource, UI::MeshResource>(obj, prop);
	}
	else if (UI::IsUIMetaclass<MetaGUI::MaterialResource>(prop))
	{
		return DrawResource<Renderer::MaterialResource, UI::MaterialResource>(obj, prop);
	}
	else if (UI::IsUIMetaclass<MetaGUI::EnumSelectOptional>(prop))
	{
		return DrawEnumSelectOptional(obj, prop);
	}
	else if (UI::IsUIMetaclass<MetaGUI::EnumSelect>(prop))
	{
		return DrawEnumSelect(obj, prop);
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
