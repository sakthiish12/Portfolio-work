// /******************************************************************************/
// /*!
// \file         VertexBufferLayout.h
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         08th Oct, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the declarations of the class Vertex Buffer 
// \lines        60 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef VERTEX_BUFFER_LAYOUT
#define VERTEX_BUFFER_LAYOUT

#include <vector>
#include <GL/glew.h>
#include "Renderer.h"

struct VertexBufferElement {
	unsigned int type;
	unsigned int count;
	unsigned char normalized;

	//getting the size
	static unsigned int GetSizeOfType(unsigned int type) {
		switch(type) {
			case GL_FLOAT: return 4;
			case GL_UNSIGNED_INT: return 4;
			case GL_UNSIGNED_BYTE: return 1;
		}
		ASSERT(false);
		return 0;
	}
};


class VertexBufferLayout {
private:
	unsigned int m_Stride;//stride for the vertices elements 
	std::vector<VertexBufferElement> m_Elements;//storing the elements
public:
	VertexBufferLayout() : m_Stride(0) {} //cstor

	template<typename T>
	void Push(unsigned int count) {
		static_assert(false);
	}
	//template specialization
	template<>
	void Push<float>(unsigned int count) {
		m_Elements.push_back({ GL_FLOAT,count,GL_FALSE });
		m_Stride += count *  VertexBufferElement::GetSizeOfType(GL_FLOAT);
	}
	template<>
	void Push<unsigned int>(unsigned int count) {
		m_Elements.push_back({ GL_UNSIGNED_INT,count,GL_FALSE });
		m_Stride += count *VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT);
	}
	template<>
	void Push<unsigned char>(unsigned int count) {
		m_Elements.push_back({ GL_UNSIGNED_BYTE,count,GL_TRUE });
		m_Stride += count*   VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE);
	}

	//for the batch rendering 
	template<>
	void Push<VertexBuffer::QuadVertex>(unsigned int count) {
		static_cast<void>(count);//unreferenced variable 
		m_Elements.push_back({ GL_FLOAT,3,GL_FALSE });//position
		m_Elements.push_back({ GL_FLOAT,4,GL_FALSE });//color 
		m_Elements.push_back({ GL_FLOAT,2,GL_FALSE });//tex coords
		m_Elements.push_back({ GL_FLOAT,1,GL_FALSE });//tex id 
		//doing up the stride 
		m_Stride += sizeof(VertexBuffer::QuadVertex::position);
		m_Stride += sizeof(VertexBuffer::QuadVertex::color);
		m_Stride += sizeof(VertexBuffer::QuadVertex::texCoords);
		m_Stride += sizeof(VertexBuffer::QuadVertex::texID);
	}
	template<>
	void Push<VertexBuffer::QuadVertexMesh>(unsigned int count) {
		static_cast<void>(count);//unreferenced variable 
		m_Elements.push_back({ GL_FLOAT,3,GL_FALSE });//position
		m_Elements.push_back({ GL_FLOAT,4,GL_FALSE });//color 
		//doing up the stride 
		m_Stride += sizeof(VertexBuffer::QuadVertexMesh::position);
		m_Stride += sizeof(VertexBuffer::QuadVertexMesh::color);
	}
	//end of batch rendering 

	inline const std::vector<VertexBufferElement>GetElements() const { return m_Elements; }

	inline unsigned int GetStride() const { return m_Stride; }
};

#endif // ! VERTEX_BUFFER_LAYOUT
