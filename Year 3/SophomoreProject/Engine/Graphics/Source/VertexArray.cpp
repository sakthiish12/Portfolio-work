// /******************************************************************************/
// /*!
// \file         VertexArray.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         08th Oct, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the definitions of the vertex array class
// \lines        28 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include <GL\glew.h>
#include "VertexArray.h"
#include "Renderer.h"

#include"VertexBufferLayout.h"


/*************************************************
* @Brief - To use a VAO all you have to do is bind the VAO using glBindVertexArray. From that point on we should bind/configure 
the corresponding VBO(s) and attribute pointer(s) and then unbind the VAO for later use. As soon as we want to draw an object, we simply bind the VAO with the preferred 
settings before drawing the object and that is it. In code this would look a bit like this:
***************************************************/
VertexArray::VertexArray() {
	
	GLCall(glGenVertexArrays(1, &m_RendererID));

}

VertexArray::~VertexArray() {
	GLCall(glDeleteVertexArrays(1, &m_RendererID));
}


void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout) {
	Bind();
	vb.Bind();
	const auto& elements = layout.GetElements();
	__int64 offset = 0; //64 bit int 
	for (unsigned int i = 0; i < elements.size(); ++i) {

		const auto& element = elements[i];
		GLCall(glEnableVertexAttribArray(i));//enable the vertex attribute array 

		GLCall(glVertexAttribPointer(i, element.count, element.type, element.normalized , layout.GetStride(),(const void*)offset )); 
		//calculating the offset based on the xyz all , stride is the space between each vertex 
		offset += static_cast<__int64>(element.count) * static_cast<__int64>(VertexBufferElement::GetSizeOfType(element.type));

	}
}

void VertexArray::Bind() const {
	GLCall(glBindVertexArray(m_RendererID));
}

void VertexArray::UnBind() const {
	GLCall(glBindVertexArray(0));
}