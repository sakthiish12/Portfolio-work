// /******************************************************************************/
// /*!
// \file         VertexArray.h 
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         08th Oct, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the declarations of the class Vertex Array 
// \lines        15 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef VERTEX_ARRAY
#define VERTEX_ARRAY

#include "VertexBuffer.h"

//forward declaration
class  VertexBufferLayout;

class VertexArray {
private:
	unsigned int m_RendererID;//renderer id 
public:

	VertexArray();//cstor 

	~VertexArray();//dstor 

	void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);//adding buffer based on the layout 

	void Bind() const;//vertex array binding 

	void UnBind() const;//vertex array clear 
};


#endif 
