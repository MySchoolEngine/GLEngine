#pragma once

#include <GUI/GUIApi.h>

#include <Utils/Reflection/Metadata.h>

#include <functional>
#include <rttr/type>

/**
 * In order to make RTTIGUI work you need to register it with proper metaclasses.
 * Also, it has to be bound as ptr.
 *
 * e.g.
 * .property("Translation", &C_Transformations::m_TranslationVec)
 * 	(
 * 		rttr::policy::prop::bind_as_ptr,
 * 		RegisterMetaclass<MetaGUI::Vec3>(),
 * 		RegisterMetamember<UI::Vec3::Name>("Translate")
 * 	)
 * If you want to have the property bound as ptr for whatever reason   you can create property alias.

 * Another option is to create custom widget for given type.
 * Than you need to register whole function.
 *
 */

namespace GLEngine::GUI {

class C_GUIManager;

// enclosingProperty is the property this instance was reached through (e.g. a MaterialResource
// field on a Material), or null for a true root call (e.g. the object an entity editor opens
// directly) - used by a registered CustomGUIWidget drawer to read a per-property label override,
// since the widget is tagged on the TYPE and wouldn't otherwise know which property it's in.
GUI_API_EXPORT std::vector<rttr::property> DrawAllPropertyGUI(rttr::instance& obj, const rttr::property* enclosingProperty = nullptr);
// return true when value changes
GUI_API_EXPORT bool			 DrawPropertyGUI(rttr::instance obj, const rttr::property& prop);
void						 SetGUIManager(C_GUIManager& mgr);
GUI_API_EXPORT C_GUIManager* GetGUIManager();

// A type tagged RegisterMetaclass<MetaGUI::CustomGUIWidget>() (see DrawAllPropertyGUI) draws
// itself via a function registered here instead of via RTTR metadata - RTTR does not merge
// type-level metadata attached from a second class_<T>() call once a type is already fully
// registered elsewhere (only members/methods merge that way), so a module that doesn't own the
// type (e.g. Editor, drawing a resource type Renderer owns and already fully registered) can't
// attach the draw function as metadata. The owning module tags its own type at its own,
// original registration; whoever implements the drawing calls this instead, at its own startup.
// A type nobody registered a drawer for (e.g. Editor isn't linked into this binary) simply draws
// nothing for it, same as an RTTR type with no properties would.
using CustomWidgetDrawFunc = std::function<bool(rttr::instance&, const rttr::property*)>;
GUI_API_EXPORT void RegisterCustomWidgetDrawer(rttr::type type, CustomWidgetDrawFunc drawFunc);

} // namespace GLEngine::GUI
