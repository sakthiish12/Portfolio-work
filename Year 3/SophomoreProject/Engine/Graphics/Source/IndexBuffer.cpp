// /******************************************************************************/
// /*!
// \file         IndexBuffer.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         08th Oct, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the declarations of the index buffer class 
// \lines        16 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include "IndexBuffer.h"

#include "Renderer.h"
IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count) : m_Count(count) {

    //ASSERT(sizeof(unsigned int) == sizeof(GLuint)); //check if the sizeof unsigned int is the same as the size of glu int 

    GLCall(glGenBuffers(1, &m_RendererID));//create buffer 
    
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID)); //the one currently being binded in drawn you need to bind before you can draw 

    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW)); // for square
}

IndexBuffer::~IndexBuffer() {
    GLCall(glDeleteBuffers(1, &m_RendererID));//delete buffer 
}

void IndexBuffer::Bind() const {
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID)); //the one currently being binded in drawn you need to bind before you can draw 
}

void IndexBuffer::UnBind() const {
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0)); //the one currently being binded in drawn you need to bind before you can draw 
}