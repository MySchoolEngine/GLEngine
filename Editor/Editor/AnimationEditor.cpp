#include <EditorStdafx.h>

#include <Editor/AnimationEditor.h>

#include <GUI/GUIUtils.h>

#include <ImSequencer.h>
#include <fmt/format.h>
#include <imgui.h>
#include <imgui_internal.h>

namespace GLEngine::Editor {

//=================================================================================
C_AnimationEditor::C_AnimationEditor()
{
	m_Tracks.emplace_back(TrackData{Renderer::C_AnimationTrack<float>(), std::string("Track1"), 0xff0000ff, false});
	for (int i = 0; i < 10; ++i)
		m_Tracks.back().track.AddKeyFrame({0.1f * i, 0.1f * i});
	m_Tracks.emplace_back(TrackData{Renderer::C_AnimationTrack<float>(), std::string("Track2"), 0xff00ff00, false});
	for (int i = 0; i < 10; ++i)
		m_Tracks.back().track.AddKeyFrame({0.1f * i, 1.f - 0.1f * i});
}

//=================================================================================
void C_AnimationEditor::DrawGUI()
{
	ImGui::Begin("Track animation");
	ImGui::BeginGroup();
	const ImGuiIO& io						 = ImGui::GetIO();
	ImDrawList*	   draw_list				 = ImGui::GetWindowDrawList();
	const auto	   canvas_pos				 = ImGui::GetCursorScreenPos();
	const auto	   canvas_size				 = ImGui::GetContentRegionAvail();
	const auto	   default_font				 = ImGui::GetFont();
	const auto	   font_size				 = ImGui::GetFontSize();
	const auto	   legendWidth				 = LegendWidth() + 6;
	const auto	   headerHeight				 = font_size;
	const auto	   trackPointColour			 = 0xffffffff;
	const auto	   trackPointMouseOverColour = 0xff0000ff;


	const ImRect rect(canvas_pos, canvas_pos + canvas_size);
	const ImRect timelineRect(canvas_pos + ImVec2(legendWidth, headerHeight), canvas_pos + canvas_size);

	ImVec2 legendPos(3, headerHeight + 2);
	DrawLegend(draw_list, canvas_pos + legendPos);

	draw_list->AddRect(rect.Min, rect.Max, 0xFFAAAAAA, 4);
	draw_list->AddLine(canvas_pos + ImVec2(legendWidth, 0), canvas_pos + ImVec2(legendWidth, canvas_size.y - 1), 0xFFAAAAAA);
	draw_list->AddLine(canvas_pos + ImVec2(0, headerHeight), canvas_pos + ImVec2(canvas_size.x, headerHeight), 0xFFAAAAAA);
	// draw timeline

	const auto numLines = 20;

	for (int i = 0; i <= numLines; ++i)
	{
		if (timelineRect.GetWidth() <= 850 && i % 2)
		{
			continue;
		}
		char	   tmps[32];
		const auto end		 = fmt::format_to(tmps, "{:.2f}", 1.0f / numLines * i);
		const auto lineX	 = legendWidth + (timelineRect.GetWidth() / static_cast<float>(numLines)) * i;
		auto	   textXPos	 = lineX;
		const auto textWidth = ImGui::CalcTextSize(tmps, end);

		if (i == 0)
			textXPos += 1; // offset one pixel from baseline
		if (i > 0)
			textXPos -= textWidth.x / 2; // center the text
		if (i == numLines)
			textXPos -= textWidth.x / 2; // last one needs to be before the line

		draw_list->AddLine(canvas_pos + ImVec2(lineX, headerHeight), canvas_pos + ImVec2(lineX, canvas_size.y - 1), (i % 2) ? 0xFFAAAAAA : 0xFFCCCCCC, (i % 2) ? 1.f : 2.f);
		draw_list->AddText(default_font, font_size, canvas_pos + ImVec2(textXPos, headerHeight - font_size), 0xAAFFFFFF, tmps, end);
	}
	const ImVec2 pointSize(4.f, 4.f);
	draw_list->PushClipRect(timelineRect.Min, timelineRect.Max);
	for (const auto& track : m_Tracks)
		for (const auto& frame : track.track)
		{
			const ImVec2 point = timelineRect.Min + (timelineRect.GetSize() * ImVec2(frame.Time.GetValue(), 1.f - frame.Value));
			const ImRect pointRect(point - pointSize / 2, point + pointSize / 2);
			const bool	 mouseOver = pointRect.Contains(io.MousePos);
			draw_list->AddRectFilled(pointRect.Min, pointRect.Max, mouseOver ? trackPointMouseOverColour : trackPointColour, pointSize.x / 2);
		}
	const int  numLineSamples = static_cast<int>(timelineRect.GetWidth()) / 20;
	const auto sampleDiff	  = 1.f / (numLineSamples);
	for (const auto& trackData : m_Tracks)
		for (int i = 0; i <= numLineSamples; ++i)
		{
			const auto	 lineStartSample = trackData.track.Sample(i * sampleDiff);
			const auto	 lineEndSample	 = trackData.track.Sample((i + 1) * sampleDiff);
			const ImVec2 lineStart		 = timelineRect.Min + (timelineRect.GetSize() * ImVec2(lineStartSample.Time.GetValue(), 1.f - lineStartSample.Value));
			const ImVec2 lineEnd		 = timelineRect.Min + (timelineRect.GetSize() * ImVec2(lineEndSample.Time.GetValue(), 1.f - lineEndSample.Value));
			draw_list->AddLine(lineStart, lineEnd, trackData.colour);
		}

	if (timelineRect.Contains(io.MousePos))
	{
		auto textXPos = io.MousePos.x;
		draw_list->AddLine(ImVec2(io.MousePos.x, headerHeight + canvas_pos.y), ImVec2(io.MousePos.x, canvas_size.y - 1 + canvas_pos.y), 0xFFAAAAAA);
		//ImGui::SetTooltip("%s: %f", trackName, m_Track.Sample((io.MousePos.x - timelineRect.Min.x) / timelineRect.GetSize().x).Value);
		// draw_list->AddText(default_font, font_size, canvas_pos + ImVec2(textXPos, headerHeight - font_size), 0xAAFFFFFF, tmps, end);
	}

	draw_list->PopClipRect();

	ImGui::EndGroup();
	ImGui::End();
}

//=================================================================================
float C_AnimationEditor::LegendWidth() const
{
	float maxLen = 0.f;
	ForEachExpandedLine([&maxLen](const TrackData& trackData) { maxLen = std::max(maxLen, ImGui::CalcTextSize(trackData.name.c_str()).x); });
	return maxLen;
}

//=================================================================================
void C_AnimationEditor::ForEachExpandedLine(std::function<void(const C_AnimationEditor::TrackData&)> f) const
{
	// TODO expanded
	for (const auto& trackData : m_Tracks)
	{
		f(trackData);
	}
}

//=================================================================================
void C_AnimationEditor::DrawLegend(ImDrawList* drawList, ImVec2 LegendBegin) const
{
	const auto font_size = ImGui::GetFontSize();
	ForEachExpandedLine([&](const auto& trackData) {
		drawList->AddText(LegendBegin, trackData.colour, trackData.name.c_str());
		LegendBegin.y += font_size + 4;
	});
}

} // namespace GLEngine::Editor
