#pragma once

#include <GLRenderer/Buffers/GLBuffer.h>

namespace GLEngine {
namespace GLRenderer {
namespace VAO {

class I_GLVAOBae {
public:
	I_GLVAOBae();
	virtual ~I_GLVAOBae();

	void bind()		const;
	void unbind()	const;
protected:
	GLuint m_id;
};

template<int BUFFERS>
class C_GLVAO : public I_GLVAOBae {
public:
	virtual ~C_GLVAO() = default;
	
	template<int INDEX>
	using T_EnableIndex = typename std::enable_if<(INDEX < BUFFERS), void>::type;

	template<int INDEX, GLenum BUFFERTYPE, class T,
			typename = T_EnableIndex<INDEX>>
	void SetBuffer(std::vector<T> data) {
		m_Buffers[INDEX] = std::make_unique<Buffers::C_GLBuffer<BUFFERTYPE>>();
		BindBuffer<INDEX>();

		const auto typeLenght = sizeof(T);

		glBufferData(BUFFERTYPE, data.size() * typeLenght, data.data(), GL_STATIC_DRAW);
		if (BUFFERTYPE != GL_ELEMENT_ARRAY_BUFFER) {
			glVertexAttribPointer(INDEX, typeLenght / sizeof(float), GL_FLOAT, GL_FALSE, 0, nullptr);
		}
		m_Buffers[INDEX]->unbind();
	}

	template<int INDEX, typename = T_EnableIndex<INDEX>>
	void BindBuffer() {
		m_Buffers[INDEX]->bind();
	}

	template<int INDEX, 
		typename = T_EnableIndex<INDEX>>
	void EnableArray() {
		glEnableVertexAttribArray(INDEX);
	}

protected:
	std::array<std::unique_ptr<Buffers::I_GLBufferBase>, BUFFERS> m_Buffers;
};
}}}
#include <GLRenderer/VAO/VAO.inl>