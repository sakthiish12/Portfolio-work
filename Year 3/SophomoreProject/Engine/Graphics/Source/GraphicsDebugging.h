// /******************************************************************************/
// /*!
// \file         GraphicsDebugging.h
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         12th Nov, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definitions for the BatchRenderer 
// \lines        123 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef GRAPHICS_DEBUG
#define GRAPHICS_DEBUG

#include "GraphicsApplication.h"
#include "../../Math/Vector/Vector2.h"//for vec2 math 
//#include "../../Math/Vector/Vector4.h"//for vec4 math 
//#include "../../Math/Vector/Vector3.h"//for vec3 math 
//#include "../../Math/MathFunctions/Mathf.h"//for the math functions
#include "../../AssetManager/AssetManager.h"//asset manager 

class GraphicsDebugging : public Renderer{
public:
	GraphicsDebugging();//cstor 
	~GraphicsDebugging();//dstor 

	GraphicsDebugging(const GraphicsDebugging&) = delete; //delete the copy constructor (singleton)
	GraphicsDebugging& operator=(const GraphicsDebugging&) = delete;//deleting the assignment operator 

	//drawing functions 
	void drawLine(const Mathf::Vector2& pt0, const Mathf::Vector2& pt1, const Mathf::Vector4& clr, const float& lineWidth);//draws line 
	void drawFilledSquare(const Mathf::Vector2& center, float radius, const Mathf::Vector4& clr);//draws filled square 
	void drawFilledRect(const Mathf::Vector2& center, const Mathf::Vector2& sca, const Mathf::Vector4& clr);//draws filled square 
	void drawLineSquare(const Mathf::Vector2& center, float radius, const Mathf::Vector4& clr);//draws a line square
	void drawLineRect(const Mathf::Vector2& center, const Mathf::Vector2& sca, const Mathf::Vector4& clr);//draws a line square
	void drawFilledCircle(const Mathf::Vector2& center, const float& radius, const Mathf::Vector4& clr);//draws a circle 
	void drawPoint(const Mathf::Vector2& pt0, const float& size, const Mathf::Vector4& clr);//draws a point on screen

	static GraphicsDebugging& GetInstance();//getting the instance of the object class 
private:
	std::vector<float> linePositions; //storing of the line positions 
	unsigned int boxIndices[6];//storing of the box indices 

	//for the draw 
	std::unique_ptr<VertexArray> circleVAO;
	std::unique_ptr<IndexBuffer> circleIBO;
	std::unique_ptr<VertexArray> squareVAO;
	std::unique_ptr<IndexBuffer> squareIBO;
	std::unique_ptr<VertexArray> lineVAO;
	std::unique_ptr<IndexBuffer> lineIBO;
	std::unique_ptr<Shader> debugShader; 


	std::vector<float> vboxMesh;//vertices of the box 
	std::vector<float> iboxMesh;//indices of the box 

	glm::mat4 calculateMVP(const Mathf::Vector2& center, const Mathf::Vector2& sca);
};

#endif