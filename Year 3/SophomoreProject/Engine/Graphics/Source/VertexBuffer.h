// /******************************************************************************/
// /*!
// \file         VertexBuffer.h
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         08th Oct, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the declarations of the vertex buffer class 
// \lines        20 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef VERTEX_BUFFER_H
#define VERTEX_BUFFER_H

class VertexBuffer {
private:
	unsigned int m_RendererID; //id that keeps track of everytime of object in opengl unique id 

public:

	//struct for quad vertex
	struct QuadVertex {
		float position[3];
		float color[4];
		float texCoords[2];
		float texID;
	};

	//struct for line vertex
	struct LineVertex {
		float position[3];
		float color[4];

		LineVertex() : position{ 0.f, 0.f, 0.f }, color{ 0.f, 0.f, 0.f } {

		}
 	};

	//struct for quad vertex 
	struct QuadVertexMesh {
		float position[3];
		float color[4];

		QuadVertexMesh() : position{ 0.f, 0.f, 0.f }, color{ 0.f, 0.f, 0.f } {

		}
	};

	//struct for triangle vertex
	struct TriangleVertexMesh {
		float position[3];
		float color[4];
	};

	VertexBuffer(const void* data, unsigned int size);

	VertexBuffer(unsigned int maxNumofQuads);//for the batching 

	~VertexBuffer();

	void Bind() const; //bind 
	void UnBind() const; //unbind 

	void SubData(const void* data, unsigned int size) const;//sub datat for dynamic draw 
	void SubDataLines(const void* data, unsigned int size) const;//sub datat for dynamic draw of lines 
	void SubDataTriangles(const void* data, unsigned int size) const;//sub datat for dynamic draw of triangles
	void SubDataQuadMesh(const void* data, unsigned int size) const;//sub data for the quad mesh 
	
};

#endif // !VERTEX_BUFFER_H
