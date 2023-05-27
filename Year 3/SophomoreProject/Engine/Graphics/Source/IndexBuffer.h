// /******************************************************************************/
// /*!
// \file         IndexBuffer.h 
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         08th Oct, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function declarations of the class index buffer  
// \lines        14 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef INDEX_BUFFER_H
#define INDEX_BUFFER_H

class IndexBuffer {
private:
	unsigned int m_RendererID; //id that keeps track of everytime of object in opengl unique id 
	unsigned int m_Count;  //count the number of indices it actually has 

public:
	IndexBuffer(const unsigned int* data, unsigned int count);

	~IndexBuffer();

	void Bind() const; //bind 
	void UnBind() const; //unbind 

	inline unsigned int GetCount() const { return m_Count;  }//getter for the count 
};

#endif // !INDEX_BUFFER_H
