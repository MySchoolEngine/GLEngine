#pragma once

#include <GUI/GUIApi.h>

#include <Utils/Reflection/Metadata.h>

#include <rttr/type>

/**
 * In order to make RTTIGUI work you need to register it with proper metaclasses.
 * Also it has to be bound as ptr.
 *
 * e.g.
 * .property("Translation", &C_Transformations::m_TranslationVec)
 * 	(
 * 		rttr::policy::prop::bind_as_ptr,
 * 		RegisterMetaclass<MetaGUI::Vec3>(),
 * 		RegisterMetamember<UI::Vec3::Name>("Translate")
 * 	)
 * If you want to have the property bound as ptr for whatever reasons you can create property alias.

 * Another option is to create custom widget for given type.
 * Than you need to register whole function.
 * 
 */

namespace GLEngine::GUI {

class C_GUIManager;

GUI_API_EXPORT std::vector<rttr::property> DrawAllPropertyGUI(rttr::instance& obj);
// return true when value changes
GUI_API_EXPORT bool DrawPropertyGUI(rttr::instance& obj, const rttr::property& prop);
void				SetGUIManager(C_GUIManager& mgr);
} // namespace GLEngine::GUI
