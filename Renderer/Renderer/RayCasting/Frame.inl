#pragma once


namespace GLEngine::Renderer {


//=================================================================================
constexpr S_Frame::S_Frame(const glm::vec3& x, const glm::vec3& y, const glm::vec3& z)
	: X(x)
	, Y(y)
	, Z(z)
{
}

//=================================================================================
constexpr S_Frame::S_Frame()
	: X(1, 0, 0)
	, Y(0, 1, 0)
	, Z(0, 0, 1)
{
}

//=================================================================================
constexpr glm::vec3 S_Frame::Bitangent() const
{
	return Z;
}

//=================================================================================
constexpr glm::vec3 S_Frame::Tangnt() const
{
	return X;
}

//=================================================================================
constexpr glm::vec3 S_Frame::Normal() const
{
	return Y;
}

//=================================================================================
constexpr glm::vec3 S_Frame::ToLocal(const glm::vec3& a) const
{
	return {glm::dot(a, X), glm::dot(a, Y), glm::dot(a, Z)};
}

//=================================================================================
constexpr glm::vec3 S_Frame::ToWorld(const glm::vec3& a) const
{
	return X * a.x + Y * a.y + Z * a.z;
}
} // namespace GLEngine::Renderer
