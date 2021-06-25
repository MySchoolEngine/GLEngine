#include <AnimationStdAfx.h>

#include <Animation/AnimationTrack.h>

namespace GLEngine::Animation {

//=================================================================================
void C_AnimationTrack::AddKeyFrame(S_AnimationFrame&& keyframe)
{
	if (empty())
	{
		m_Timeline.emplace_back(keyframe);
		return;
	}
	auto previousFrame = PreviousFrame(keyframe.Time);
	if (previousFrame->Time < keyframe.Time)
	{
		previousFrame++;
	}
	m_Timeline.insert(previousFrame, keyframe);
}

//=================================================================================
void C_AnimationTrack::PushBackFrame(S_AnimationFrame&& keyframe)
{
	GLE_ASSERT(m_Timeline.empty() || keyframe.Time >= m_Timeline.back().Time, "Adding keyframes out of order not suppoerted");
	m_Timeline.emplace_back(keyframe);
}

//=================================================================================
S_AnimationFrame C_AnimationTrack::Sample(const S_Timestamp time) const
{
	if (empty())
	{
		return S_AnimationFrame();
	}

	const auto previousFrame = PreviousFrame(time);
	auto	   nextFrame	 = std::next(previousFrame);

	S_Timestamp intervalLen;
	if (nextFrame == m_Timeline.end())
	{
		intervalLen = 1.0f - previousFrame->Time.GetValue();
		nextFrame	= previousFrame;
	}
	else
	{
		intervalLen = nextFrame->Time - previousFrame->Time;
	}

	const auto timeInInterval	  = time - previousFrame->Time;
	const auto percentageProgress = timeInInterval.GetValue() / intervalLen.GetValue();

	return S_AnimationFrame{glm::mix(previousFrame->Value, nextFrame->Value, percentageProgress), time};
}

//=================================================================================
S_AnimationFrame& C_AnimationTrack::operator[](const std::size_t index)
{
	return m_Timeline[index];
}

//=================================================================================
const S_AnimationFrame& C_AnimationTrack::operator[](const std::size_t index) const
{
	return m_Timeline[index];
}

//=================================================================================
std::size_t C_AnimationTrack::size() const
{
	return m_Timeline.size();
}

//=================================================================================
bool C_AnimationTrack::empty() const
{
	return m_Timeline.empty();
}

//=================================================================================
C_AnimationTrack::T_Iterator C_AnimationTrack::begin()
{
	return m_Timeline.begin();
}

//=================================================================================
C_AnimationTrack::T_ConstIterator C_AnimationTrack::begin() const
{
	return m_Timeline.begin();
}

//=================================================================================
C_AnimationTrack::T_Iterator C_AnimationTrack::end()
{
	return m_Timeline.end();
}

//=================================================================================
C_AnimationTrack::T_ConstIterator C_AnimationTrack::end() const
{
	return m_Timeline.end();
}

//=================================================================================
C_AnimationTrack::T_ConstIterator C_AnimationTrack::PreviousFrame(const S_Timestamp time) const
{
	auto it = std::upper_bound(m_Timeline.begin(), m_Timeline.end(), time, [](const S_Timestamp& val, const S_AnimationFrame& other) { return other.Time > val; });
	if (it != begin())
	{
		it--;
		return it;
	}
	return it;
}

} // namespace GLEngine::Animation
