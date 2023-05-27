// /******************************************************************************/
// /*!
// \file         GraphicsApplication.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         12th Nov, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definitions for the graphics definitions
// \lines        123 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include "../../pch.h"
#include "GraphicsApplication.h"
#include "Renderer.h"
void Graphic_Application::initalize_graphics(void)
{
    initOpenGLGLEW();
   
    Graphic_Application::PrintingGPUSpecs();

    //enable depth testing 
    GLCall(glEnable(GL_DEPTH_TEST));
    GLCall(glDepthFunc(GL_LEQUAL));//depth testing passes if the fragment's depth value is less than the stored depth value 

    ////blending to get rid of the white 
    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    graphicsInitialized = true;
}

void Graphic_Application::initOpenGLGLEW()
{
    //glewExperimental = GL_TRUE; // start GLEW extension library
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        std::cerr << "Error: Unable to initialize GLEW " << glewGetErrorString(err) << std::endl;
        std::exit(EXIT_FAILURE);
    }

    if (!GLEW_VERSION_4_3) { // check support for core GL 4.3 ...
        std::cout << "Error: Cannot access GL 4.6 API" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    std::cout << "Rendering Environment Success" << std::endl;
}
/*!*************************************************************************
    ****
    \brief
    This function prints out the necessary gpu specs
    \param void
    \return void
****************************************************************************
***/
void Graphic_Application::PrintingGPUSpecs() {
    // print GPU specs ...
    GLubyte const* str_ven = glGetString(GL_VENDOR); // vendor
    GLubyte const* str_rend = glGetString(GL_RENDERER); // opengl renderer
    GLubyte const* str_vergl = glGetString(GL_VERSION); // opengl version number 
    GLubyte const* str_verglsl = glGetString(GL_SHADING_LANGUAGE_VERSION); // glsl version number 

    //printing out 
    std::cout << "***START OF GRAPHICS INFO***" << std::endl;
    std::cout << "GPU Vendor: " << str_ven << std::endl;
    std::cout << "GL Renderer: " << str_rend << std::endl;
    std::cout << "GL Version: " << str_vergl << std::endl;
    std::cout << "GL Shader Version: " << str_verglsl << std::endl;

    //getting the relevant specifications 
    GLint majorver;
    glGetIntegerv(GL_MAJOR_VERSION, &majorver); //major ver
    GLint minorver;
    glGetIntegerv(GL_MINOR_VERSION, &minorver); //minor ver
    GLboolean doublebuffer;
    glGetBooleanv(GL_DOUBLEBUFFER, &doublebuffer); //double buffer 
    GLint maxvert;
    glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &maxvert); //max vertices
    GLint maxind;
    glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &maxind);//max indices
    GLint maxtexsize;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxtexsize);//max texture size
    GLint maxviewportdim[2];
    glGetIntegerv(GL_MAX_VIEWPORT_DIMS, maxviewportdim);//max viewport dimension
    GLint maxvertattrib;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxvertattrib); //max vertex attribute
    GLint maxvertbind;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIB_BINDINGS, &maxvertbind); //max vertex binding attribute
    GLint maxTextureSlots;
    glGetIntegerv(GL_MAX_TEXTURE_UNITS, &maxTextureSlots); //max texture slots/units

    //printing out 
    std::cout << "GL Major Version: " << majorver << std::endl;
    std::cout << "GL Minor Version: " << minorver << std::endl;
    if (doublebuffer) {
        std::cout << "Current OpenGL Context is double buffered" << std::endl;
    }
    else {
        std::cout << "Current OpenGL Content is NOT double buffered" << std::endl;
    }
    std::cout << "Maximum Vertex Count: " << maxvert << std::endl;
    std::cout << "Maximum Indices Count: " << maxind << std::endl;
    std::cout << "Maximum Viewport Dimensions: " << maxviewportdim[0] << " x " << maxviewportdim[1] << std::endl;
    std::cout << "Maximum number of Generic Vertex Attributes: " << maxvertattrib << std::endl;
    std::cout << "Maximum number of Vertex Buffer Bindings: " << maxvertbind << std::endl;
    std::cout << "GL Maximum Texture Size: " << maxtexsize << std::endl;
    std::cout << "Maximum number of Texture Units: " << maxTextureSlots << std::endl;
    std::cout << "***END OF GRAPHICS INFO***" << std::endl << std::endl;
}


Graphic_Application& Graphic_Application::GetInstance() {
    static Graphic_Application instance;
    return instance;
}
