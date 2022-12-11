#pragma once

#include <glm/gtx/type_trait.hpp>

namespace GLEngine::Renderer {

//=================================================================================
template <class valT> void C_AnimationTrack<valT>::AddKeyFrame(C_AnimationTrack<valT>::T_Frame&& keyframe)
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
template <class valT> void C_AnimationTrack<valT>::PushBackFrame(C_AnimationTrack<valT>::T_Frame&& keyframe)
{
	GLE_ASSERT(m_Timeline.empty() || keyframe.Time >= m_Timeline.back().Time, "Adding keyframes out of order not suppoerted");
	m_Timeline.emplace_back(keyframe);
}

//=================================================================================
template <class valT> typename C_AnimationTrack<valT>::T_Frame C_AnimationTrack<valT>::Sample(const S_Timestamp time) const
{
	if (empty())
	{
		return T_Frame();
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

	return T_Frame{glm::mix(previousFrame->Value, nextFrame->Value, percentageProgress), time};
}

//=================================================================================
template <class valT> typename C_AnimationTrack<valT>::T_Frame& C_AnimationTrack<valT>::operator[](const std::size_t index)
{
	return m_Timeline[index];
}

//=================================================================================
template <class valT> const typename C_AnimationTrack<valT>::T_Frame& C_AnimationTrack<valT>::operator[](const std::size_t index) const
{
	return m_Timeline[index];
}

//=================================================================================
template <class valT> std::size_t C_AnimationTrack<valT>::size() const
{
	return m_Timeline.size();
}

//=================================================================================
template <class valT> bool C_AnimationTrack<valT>::empty() const
{
	return m_Timeline.empty();
}

//=================================================================================
template <class valT> typename C_AnimationTrack<valT>::T_Iterator C_AnimationTrack<valT>::begin()
{
	return m_Timeline.begin();
}

//=================================================================================
template <class valT> typename C_AnimationTrack<valT>::T_ConstIterator C_AnimationTrack<valT>::begin() const
{
	return m_Timeline.begin();
}

//=================================================================================
template <class valT> typename C_AnimationTrack<valT>::T_Iterator C_AnimationTrack<valT>::end()
{
	return m_Timeline.end();
}

//=================================================================================
template <class valT> typename C_AnimationTrack<valT>::T_ConstIterator C_AnimationTrack<valT>::end() const
{
	return m_Timeline.end();
}

//=================================================================================
template <class valT> typename C_AnimationTrack<valT>::T_ConstIterator C_AnimationTrack<valT>::PreviousFrame(const S_Timestamp time) const
{
	auto it = std::upper_bound(m_Timeline.begin(), m_Timeline.end(), time, [](const S_Timestamp& val, const T_Frame& other) { return other.Time > val; });
	if (it != begin())
	{
		it--;
		return it;
	}
	return it;
}

//=================================================================================
template <class valT> constexpr std::size_t C_AnimationTrack<valT>::GetNumVariables() const
{
	if constexpr (std::is_floating_point_v<valT>)
		return 1;
	else if constexpr (glm::type<valT>::is_vec)
		return valT::length();
	else if constexpr (glm::type<valT>::is_quat)
		return 4;
}

} // namespace GLEngine::Renderer
