// /******************************************************************************/
// /*!
// \file         VertexBuffer.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         08th Oct, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the definitions of the Vertex buffer class
// \lines        25 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include "VertexBuffer.h"
#include "Renderer.h"
#include <GL/glew.h>


VertexBuffer::VertexBuffer(const void* data, unsigned int size) {
    GLCall(glGenBuffers(1, &m_RendererID));
    //selecting the buffer
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID)); //the one currently being binded in drawn you need to bind before you can draw 

    GLCall(glBufferData(GL_ARRAY_BUFFER,size, data, GL_STATIC_DRAW)); // for static vertex buffer

}


//for the batch rendering 
VertexBuffer::VertexBuffer(unsigned int maxNumofQuads) {

    GLCall(glGenBuffers(1, &m_RendererID));
    //selecting the buffer
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID)); //the one currently being binded in drawn you need to bind before you can draw 

    GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(QuadVertex) * maxNumofQuads, nullptr, GL_DYNAMIC_DRAW));//dynamic geomtry rendering harded to store max of 1000 vertex TO DO : create a batch rendering class 

}


VertexBuffer::~VertexBuffer() {
    GLCall(glDeleteBuffers(1, &m_RendererID));//clearing buffer 
}

void VertexBuffer::Bind() const {
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID)); //the one currently being binded in drawn you need to bind before you can draw 
}

void VertexBuffer::UnBind() const {
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0)); //the one currently being binded in drawn you need to bind before you can draw 
}

void VertexBuffer::SubData(const void* data, unsigned int size) const {
    //GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, size * sizeof(unsigned int), data));//passing the sub data into the buffer 
    GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, size * sizeof(VertexBuffer::QuadVertex), data));//passing the sub data into the buffer 

}

void VertexBuffer::SubDataLines(const void* data, unsigned int size) const {
    //GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, size * sizeof(unsigned int), data));//passing the sub data into the buffer 
    GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, size * sizeof(VertexBuffer::LineVertex), data));//passing the sub data into the buffer 

}

void VertexBuffer::SubDataTriangles(const void* data, unsigned int size) const {
    //GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, size * sizeof(unsigned int), data));//passing the sub data into the buffer 
    GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, size * sizeof(VertexBuffer::TriangleVertexMesh), data));//passing the sub data into the buffer 

}



void VertexBuffer::SubDataQuadMesh(const void* data, unsigned int size) const {
    //GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, size * sizeof(unsigned int), data));//passing the sub data into the buffer 
    GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, size * sizeof(VertexBuffer::QuadVertexMesh), data));//passing the sub data into the buffer 

}