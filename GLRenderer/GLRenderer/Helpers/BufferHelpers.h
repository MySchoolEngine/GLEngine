#pragma once

#include <GLRendererStdafx.h>

#include <Renderer/Buffer.h>

#include <Core/CoreMacros.h>

namespace GLEngine::GLRenderer {
inline GLenum GetBufferUsage(const Renderer::BufferDynamics dynamics, const Renderer::BufferAccess access)
{
	switch (dynamics)
	{
	case Renderer::Stream:
		switch (access)
		{
		case Renderer::Draw:
			return GL_STREAM_DRAW;
		case Renderer::Read:
			return GL_STREAM_READ;
		case Renderer::Copy:
			return GL_STREAM_COPY;
		}
	case Renderer::Static:
		switch (access)
		{
		case Renderer::Draw:
			return GL_STATIC_DRAW;
		case Renderer::Read:
			return GL_STATIC_READ;
		case Renderer::Copy:
			return GL_STATIC_COPY;
		}
	case Renderer::Dynamic:
		switch (access)
		{
		case Renderer::Draw:
			return GL_DYNAMIC_DRAW;
		case Renderer::Read:
			return GL_DYNAMIC_READ;
		case Renderer::Copy:
			return GL_DYNAMIC_COPY;
		}
	}
	GLE_ASSERT(false, "Unknown type");
	return GL_STATIC_DRAW;
}

inline GLenum GetBufferUsage(const Renderer::E_BufferType type)
{
	switch (type)
	{
	case Renderer::VertexAttribute:
		return GL_ARRAY_BUFFER;
	case Renderer::Uniform:
		return GL_UNIFORM_BUFFER;
	}
	GLE_ASSERT(false, "Unknown type");
	return GL_ARRAY_BUFFER;
}
} // namespace GLEngine::GLRenderer
