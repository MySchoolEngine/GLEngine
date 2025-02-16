#include <RendererStdafx.h>

#include <Renderer/Animation/Skeleton.h>
#include <Renderer/DebugDraw.h>

#include <Physics/Primitives/Frustum.h>

namespace GLEngine::Renderer {

//=================================================================================
I_DebugDraw::~I_DebugDraw() = default;

//=================================================================================
void I_DebugDraw::DrawBone(const glm::vec3& position, const glm::vec3& dest)
{
	// TODO: missing orientation information
	const glm::vec3 boneOffset = dest - position;

	const float bumpFactor	 = .2f;
	const auto	BumpPosition = position + boneOffset * bumpFactor;

	glm::vec3 tangent;

	auto c1 = glm::cross(boneOffset, glm::vec3(0.0, 0.0, 1.0));
	auto c2 = glm::cross(boneOffset, glm::vec3(0.0, 1.0, 0.0));

	if (glm::length(c1) > glm::length(c2))
	{
		tangent = c1;
	}
	else
	{
		tangent = c2;
	}

	tangent				 = glm::normalize(tangent);
	const auto bitangent = glm::normalize(glm::cross(tangent, boneOffset));

	const float		bumpSize = .09f * glm::length(boneOffset);
	const glm::vec3 Offset1	 = tangent * bumpSize;
	const glm::vec3 Offset2	 = bitangent * bumpSize;


	DrawLine(position, BumpPosition, Colours::white);
	DrawLine(BumpPosition, dest, Colours::green);

	// square around the bump
	DrawLine(BumpPosition + Offset1 + Offset2, BumpPosition + Offset1 - Offset2, Colours::gray);
	DrawLine(BumpPosition + Offset1 + Offset2, BumpPosition - Offset1 + Offset2, Colours::gray);
	DrawLine(BumpPosition - Offset1 + Offset2, BumpPosition - Offset1 - Offset2, Colours::gray);
	DrawLine(BumpPosition + Offset1 - Offset2, BumpPosition - Offset1 - Offset2, Colours::gray);

	// pos to bump
	DrawLine(position, BumpPosition + Offset1 + Offset2, Colours::gray);
	DrawLine(position, BumpPosition + Offset1 - Offset2, Colours::gray);
	DrawLine(position, BumpPosition - Offset1 - Offset2, Colours::gray);
	DrawLine(position, BumpPosition - Offset1 + Offset2, Colours::gray);

	// bump to dest
	DrawLine(dest, BumpPosition + Offset1 + Offset2, Colours::gray);
	DrawLine(dest, BumpPosition + Offset1 - Offset2, Colours::gray);
	DrawLine(dest, BumpPosition - Offset1 - Offset2, Colours::gray);
	DrawLine(dest, BumpPosition - Offset1 + Offset2, Colours::gray);
}

//=================================================================================
void I_DebugDraw::DrawSkeleton(const glm::vec3& root, const C_Skeleton& skeleton, const glm::mat4& modelMat)
{
	const auto		locTransformation = modelMat * glm::inverse((skeleton.GetRoot().m_InverseBindTransform));
	const glm::vec4 modelDest		  = (locTransformation * glm::vec4(0.f, 0.f, 0.0f, 1.f));
	const glm::vec3 pos				  = glm::vec3(modelDest / modelDest.w);

	for (const auto& child : skeleton.GetRoot().m_Children)
	{
		const S_Joint* const joint			  = skeleton.GetJoint(child);
		const auto			 MSTransformation = modelMat * glm::inverse(joint->m_InverseBindTransform);
		const glm::vec4		 modelDest		  = MSTransformation * glm::vec4(0.f, 0.f, 0.0f, 1.f);
		const glm::vec3		 dest			  = glm::vec3(modelDest / modelDest.w);

		DrawBone(pos, dest);
	}
}

//=================================================================================
void I_DebugDraw::DrawFrustum(const Physics::Primitives::C_Frustum& frust, const Colours::T_Colour& color)
{
	const auto& position = frust.GetPosition();
	const auto& upVector = frust.GetUpVector();
	const auto& forward	 = frust.GetForeward();
	const auto	fnear	 = frust.GetNear();
	const auto	ffar	 = frust.GetFar();
	const auto	fov		 = frust.GetFov();
	const auto	aspect	 = frust.GetAspect();

	const glm::vec3 nearCenter = glm::vec3(position + (forward * fnear));
	const glm::vec3 farCenter  = glm::vec3(position + (forward * ffar));

	const glm::vec3 left = glm::cross(upVector, forward);

	const float tanHalfHFOV = tanf(glm::radians(fov / 2.0f));
	const float tanHalfVFOV = tanf(glm::radians((fov * aspect) / 2.0f));

	const float xn = fnear * tanHalfVFOV;
	const float xf = ffar * tanHalfVFOV;
	const float yn = fnear * tanHalfHFOV;
	const float yf = ffar * tanHalfHFOV;

	const auto nlt = glm::vec4(nearCenter + (xn * left) + upVector * yn, 1.0f);
	const auto nrt = glm::vec4(nearCenter - (xn * left) + upVector * yn, 1.0f);
	const auto nlb = glm::vec4(nearCenter + (xn * left) - upVector * yn, 1.0f);
	const auto nrb = glm::vec4(nearCenter - (xn * left) - upVector * yn, 1.0f);
	const auto flt = glm::vec4(farCenter + (xf * left) + upVector * yf, 1.0f);
	const auto frt = glm::vec4(farCenter - (xf * left) + upVector * yf, 1.0f);
	const auto flb = glm::vec4(farCenter + (xf * left) - upVector * yf, 1.0f);
	const auto frb = glm::vec4(farCenter - (xf * left) - upVector * yf, 1.0f);

	std::vector<glm::vec4> lines;
	lines.push_back(nlt);
	lines.push_back(nrt);
	lines.push_back(nlb);
	lines.push_back(nrb);
	lines.push_back(nlt);
	lines.push_back(nlb);
	lines.push_back(nrt);
	lines.push_back(nrb);

	lines.push_back(flt);
	lines.push_back(frt);
	lines.push_back(flb);
	lines.push_back(frb);
	lines.push_back(flt);
	lines.push_back(flb);
	lines.push_back(frt);
	lines.push_back(frb);

	lines.push_back(nlt);
	lines.push_back(flt);
	lines.push_back(nrt);
	lines.push_back(frt);
	lines.push_back(nlb);
	lines.push_back(flb);
	lines.push_back(nrb);
	lines.push_back(frb);

	DrawLines(lines, color);
	DrawPoint(position);
	DrawLine(position, position + forward, Colours::green);
	DrawLine(position, position + upVector, Colours::red);
}

//=================================================================================
void I_DebugDraw::DrawGrid(const glm::vec4& origin, unsigned short linesToSide, const glm::mat4& modelMatrix /*= glm::mat4(1.0f)*/)
{
	int limit = linesToSide;
	// cross for center
	DrawLine(origin + glm::vec4(-limit, 0, 0, 1.f), origin + glm::vec4(limit, 0, 0, 1.f));
	DrawLine(origin + glm::vec4(0, 0, -limit, 1.f), origin + glm::vec4(0, 0, limit, 1.f));

	for (int i = 1; i <= limit; ++i)
	{
		// lines in positive direction from origin
		DrawLine(origin + glm::vec4(-limit, 0, i, 1.f), origin + glm::vec4(limit, 0, i, 1.f));
		DrawLine(origin + glm::vec4(i, 0, -limit, 1.f), origin + glm::vec4(i, 0, limit, 1.f));

		// lines in negative direction from origin
		DrawLine(origin + glm::vec4(-limit, 0, -i, 1.f), origin + glm::vec4(limit, 0, -i, 1.f));
		DrawLine(origin + glm::vec4(-i, 0, -limit, 1.f), origin + glm::vec4(-i, 0, limit, 1.f));
	}
}

} // namespace GLEngine::Renderer
