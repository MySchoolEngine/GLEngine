#pragma once

#include <Editor/EditorApi.h>

#include <Animation/AnimationTrack.h>

namespace GLEngine::Editor {
class EDITOR_API_EXPORT C_AnimationEditor {
public:
	C_AnimationEditor();

	void DrawGUI();

private:
	[[nodiscard]] float LegendWidth() const;

	struct TrackData {
		Animation::C_AnimationTrack track;
		std::string					name;
		unsigned int				colour;
		bool						selected;
	};
	std::vector<TrackData>		m_Tracks; // TODO DI
};
} // namespace GLEngine::Editor
