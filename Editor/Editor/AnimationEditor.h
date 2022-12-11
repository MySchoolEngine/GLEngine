#pragma once

#include <Editor/EditorApi.h>

#include <Renderer/Animation/AnimationTrack.h>

struct ImDrawList;
struct ImVec2;

namespace GLEngine::Editor {
class EDITOR_API_EXPORT C_AnimationEditor {
public:
	C_AnimationEditor();

	void DrawGUI();

private:
	struct TrackData {
		Renderer::C_AnimationTrack<float> track;
		std::string						   name;
		unsigned int					   colour;
		bool							   selected;
	};

	[[nodiscard]] float LegendWidth() const;

	void DrawLegend(ImDrawList* drawList, ImVec2 LegendBegin) const;
	void ForEachExpandedLine(std::function<void(const TrackData&)>) const;

	std::vector<TrackData> m_Tracks; // TODO DI
};
} // namespace GLEngine::Editor
