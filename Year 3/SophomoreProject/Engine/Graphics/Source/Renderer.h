// /******************************************************************************/
// /*!
// \file         Renderer.h 
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         08th Oct, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the declarations of the class renderer 
// \lines        31 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef RENDERER_H
#define RENDERER_H

//need to define to use the static glew
#define GLEW_STATIC
#include <GL/glew.h>
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"



#define ASSERT(x) if (!(x)) __debugbreak();//assertion
// the \ ignores the newline character so you can write on a new line with no space behind!
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x,__FILE__,__LINE__))

//clearing all the gl errors
void GLClearError();

//printing out the errors that you are getting from opengl
bool GLLogCall(const char* function, const char* file, int line);

class Renderer {
public:
    Renderer() = default;//cstor 
    ~Renderer() = default;//dstor 
    Renderer(const Renderer&) = delete; //delete the copy constructor (singleton)
    Renderer& operator=(const Renderer&) = delete; //delete the assignment operator (singleton) 

    static Renderer& GetInstance();//getting the instance of the class 

    void Clear() const;
    void sDraw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;//draw function for quads
    void sDrawTriangle(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;//draw function for triangles
    void lineDraw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;//draw function for lines 

    struct Renderer2DData {
        const uint32_t maxQuads = 10000;//maximum number of quads to draw 
        const uint32_t maxVertices = maxQuads * 4 ;//maximum number of vertices to draw
        const uint32_t maxIndices = maxQuads * 6;//maximum number of indices to draw 
    };
    static Renderer2DData render_Data;//static renderer data 

private:

};

#endif // !RENDERER_H
