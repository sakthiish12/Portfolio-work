// /******************************************************************************/
// /*!
// \file         GraphicsApplication.h
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         12th Nov, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function declarations for the graphics application
// \lines        123 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef GRAPHICS_APP
#define GRAPHICS_APP

//graphics application enclosed in name space 
class Graphic_Application {
public:
	void initalize_graphics(); //the function for the graphics application at init 

	void initOpenGLGLEW();

	void PrintingGPUSpecs(); //printing of the gpu specs

	static Graphic_Application& GetInstance();//singleton get instance of class 

	bool graphicsInitialized = false;
};

#endif
