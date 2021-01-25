#pragma once

#include <GLRenderer/Buffers/GLBuffer.h>
#include <GLRenderer/Helpers/OpenGLTypesHelpers.h>

namespace GLEngine::GLRenderer::VAO {

class I_GLVAOBase {
public:
	I_GLVAOBase();
	virtual ~I_GLVAOBase();

	void bind() const;
	void unbind() const;

protected:
	GLuint m_id;
};

template <int BUFFERS> class C_GLVAO : public I_GLVAOBase {
public:
	virtual ~C_GLVAO() = default;

	template <int INDEX> using T_EnableIndex = typename std::enable_if<(INDEX < BUFFERS), void>::type;

	template <int INDEX, GLenum BUFFERTYPE, class T, typename = T_EnableIndex<INDEX>> void SetBuffer(const std::vector<T>& data)
	{
		m_Buffers[INDEX] = std::make_unique<Buffers::C_GLBuffer<BUFFERTYPE>>();
		BindBuffer<INDEX>();

		if (data.size())
			InnerSetBufferData<INDEX, BUFFERTYPE>(data);
		if (BUFFERTYPE != GL_ELEMENT_ARRAY_BUFFER)
		{
			constexpr auto typeLenght = sizeof(T);
			if constexpr (T_GLTypeIsIntegral_v<T_TypeToGL<T>::value>)
			{
				glVertexAttribIPointer(INDEX, T_GLNumComponenets_v<T>, T_TypeToGL<T>::value, 0, nullptr);
			}
			else if (T_GLTypeIsDouble_v<T_TypeToGL<T>::value>)
			{
				glVertexAttribLPointer(INDEX, T_GLNumComponenets_v<T>, T_TypeToGL<T>::value, 0, nullptr);
			}
			else
			{
				glVertexAttribPointer(INDEX, T_GLNumComponenets_v<T>, T_TypeToGL<T>::value, GL_FALSE, 0, nullptr);
			}
		}
		m_Buffers[INDEX]->unbind();
	}

	template <int INDEX, class T, typename = T_EnableIndex<INDEX>> void SetIndexBuffer(std::vector<T> data)
	{
		SetBuffer<INDEX, GL_ELEMENT_ARRAY_BUFFER>(data);
		NameBuffer<INDEX>("indices");
	}

	template <int INDEX, GLenum BUFFERTYPE, class T, typename = T_EnableIndex<INDEX>> void SetBufferData(const std::vector<T>& data, bool dynamicDraw = false)
	{
		BindBuffer<INDEX>();
		InnerSetBufferData<INDEX, BUFFERTYPE>(data, dynamicDraw);
		m_Buffers[INDEX]->unbind();
	}

	template <int INDEX, typename = T_EnableIndex<INDEX>> void BindBuffer() { m_Buffers[INDEX]->bind(); }

	template <int INDEX, typename = T_EnableIndex<INDEX>> void EnableArray() { glEnableVertexAttribArray(INDEX); }

	void NameArray(const std::string& name) { glObjectLabel(GL_VERTEX_ARRAY, m_id, static_cast<GLsizei>(name.length()), name.c_str()); }

	template <int INDEX, typename = T_EnableIndex<INDEX>> void NameBuffer(const std::string& name) { m_Buffers[INDEX]->NameBuffer(name); }

protected:
	std::array<std::unique_ptr<Buffers::I_GLBufferBase>, BUFFERS> m_Buffers;

private:
	template <int INDEX, GLenum BUFFERTYPE, class T, typename = T_EnableIndex<INDEX>> void InnerSetBufferData(const std::vector<T>& data, bool dynamicDraw = false)
	{
		constexpr auto typeLenght = sizeof(T);
		const auto	   usage	  = dynamicDraw ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;
		glBufferData(BUFFERTYPE, data.size() * typeLenght, data.data(), usage);
	}
};
} // namespace GLEngine::GLRenderer::VAO
#include <GLRenderer/VAO/VAO.inl>