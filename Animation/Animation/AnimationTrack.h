#pragma once

#include <Animation/AnimationApi.h>
#include <Animation/AnimationFrame.h>

namespace GLEngine::Animation {
class ANIMATION_API_EXPORT C_AnimationTrack {
	using T_Container	  = std::vector<S_AnimationFrame>;
	using T_Iterator	  = T_Container::iterator;
	using T_ConstIterator = T_Container::const_iterator;

public:
	C_AnimationTrack() = default;

	void						   AddKeyFrame(S_AnimationFrame&& keyframe);
	void						   PushBackFrame(S_AnimationFrame&& keyframe);
	[[nodiscard]] S_AnimationFrame Sample(const S_Timestamp time) const;


	// allow iteration and container-like structure of class
	[[nodiscard]] S_AnimationFrame&		  operator[](const std::size_t index);
	[[nodiscard]] const S_AnimationFrame& operator[](const std::size_t index) const;

	[[nodiscard]] std::size_t	  size() const;
	[[nodiscard]] bool			  empty() const;
	[[nodiscard]] T_Iterator	  begin();
	[[nodiscard]] T_Iterator	  end();
	[[nodiscard]] T_ConstIterator begin() const;
	[[nodiscard]] T_ConstIterator end() const;

//private:
	[[nodiscard]] T_ConstIterator PreviousFrame(const S_Timestamp time) const;

	T_Container m_Timeline;
};
} // namespace GLEngine::Animation