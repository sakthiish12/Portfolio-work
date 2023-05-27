// /******************************************************************************/
// /*!
// \file         Renderer.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         12th Nov, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definitions for the renderer
// \lines        123 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include "Renderer.h"
#include <iostream>


//clearing all the gl errors
void GLClearError() {

    //if it's running on release, don't show any gl error
#ifdef _DEBUG
    while (glGetError() != GL_NO_ERROR) { std::cout << "GL ERROR! " << glGetError() << std::endl; };
#endif
};

//printing out the errors that you are getting from opengl
bool GLLogCall(const char* function, const char* file, int line) {
#ifndef _DEBUG
    static_cast<void>(function);
    static_cast<void>(file);
    static_cast<void>(line);
#endif //  _DEBUG


#ifdef _DEBUG
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL Error] (" << error << "): " << function << " " << file << ":" << line << std::endl;
        return false; //not successful 
    }
#endif
    return true; //successful 
}

void Renderer::sDraw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const {
    //bind shader 
    shader.Bind(); // telling the gpu what to actually do 

    //vertex array bind 
    va.Bind(); //the array that contains all the actual data , vertex buffer and index buffer 

    //index buffer bind 
    ib.Bind(); //contains the indices 

    
    GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr)); //this is the function to be using most of the time to draw the elements
}

void Renderer::lineDraw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const {

    //bind shader
    shader.Bind();//telling the gpu what to actually do 

    //vertex array bind 
    va.Bind(); //the array that contains all the actual data , vertex buffer and index buffer 

    ib.GetCount();//contains the indices 

    //draw call 
    glLineWidth(2.f); //width of the line 
    glDrawArrays(GL_LINES, 0, ib.GetCount());//draws lines

}

void Renderer::sDrawTriangle(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const {
    //bind shader 
    shader.Bind(); // telling the gpu what to actually do 

    //vertex array bind 
    va.Bind(); //the array that contains all the actual data , vertex buffer and index buffer 

    //index buffer bind 
    ib.Bind(); //contains the indices 


    GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr)); //this is the function to be using most of the time to draw the elements
}

void Renderer::Clear() const {
    /* Render here */
    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

Renderer& Renderer::GetInstance() {
    static Renderer instance;
    return instance;
}