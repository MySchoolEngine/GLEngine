#pragma once

#include <Renderer/Animation/AnimationFrame.h>
#include <Renderer/RendererApi.h>

namespace GLEngine::Renderer {
template <class valT> class C_AnimationTrack {
	using T_Frame		  = S_AnimationFrame<valT>;
	using T_Container	  = std::vector<T_Frame>;
	using T_Iterator	  = typename T_Container::iterator;
	using T_ConstIterator = typename T_Container::const_iterator;

public:
	C_AnimationTrack() = default;

	void				  AddKeyFrame(T_Frame&& keyframe);
	void				  PushBackFrame(T_Frame&& keyframe);
	[[nodiscard]] T_Frame Sample(const S_Timestamp time) const;


	// allow iteration and container-like structure of class
	[[nodiscard]] T_Frame&		 operator[](const std::size_t index);
	[[nodiscard]] const T_Frame& operator[](const std::size_t index) const;

	[[nodiscard]] std::size_t	  size() const;
	[[nodiscard]] bool			  empty() const;
	[[nodiscard]] T_Iterator	  begin();
	[[nodiscard]] T_Iterator	  end();
	[[nodiscard]] T_ConstIterator begin() const;
	[[nodiscard]] T_ConstIterator end() const;

	[[nodiscard]] constexpr std::size_t GetNumVariables() const;

private:
	[[nodiscard]] T_ConstIterator PreviousFrame(const S_Timestamp time) const;

	T_Container m_Timeline;
};

} // namespace GLEngine::Renderer

#include <Renderer/Animation/AnimationTrack.inl>