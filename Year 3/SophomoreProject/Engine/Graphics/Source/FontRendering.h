// /******************************************************************************/
// /*!
// \file         FontRendering.h
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         12th Nov, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function declaration for the font rendering 
// \lines        123 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef FONTRENDER_H
#define FONTRENDER_H
#include <memory>
#include <map>
#include <ft2build.h>
#include FT_FREETYPE_H 
#include "Shader.h"//shader
#include "../../Math/Vector/Vector3.h"//vec3 

namespace TextRendering {

    struct Character
    {
        unsigned int TextureID;  // ID handle of the glyph texture
        glm::ivec2   Size;       // Size of glyph
        glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
        unsigned int Advance;    // Offset to advance to next glyph
    };

    struct Text
    {
        std::map<char, Character*> characterMap; // map of characters (char to be rendered)
    };

    class TextRenderer
    {
    private:

        FT_Library ft;
        std::map<std::string, Text*> textMap;

    public:
       
        TextRenderer(const std::string& ShaderFilePath = "Graphics/Resources/shaders/Font.shader");


        ~TextRenderer();

        inline static TextRenderer& GetInstance() { static TextRenderer instance; return instance; }; //singleton instance 

        void InitFont(std::string filePath, std::string fontName = "default");//init font rendering 

        void RenderText(std::string text, glm::vec2 pos, float scale, glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f), std::string fontName = "default");//render the text 
        void RenderTextMVP(std::string text, glm::vec2 pos, float scale,const float& cameraHeight,const Mathf::vec3& cameraPos, glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f), std::string fontName = "default");//render the text with varying MVP
        void RenderTextMVPMatrix(const std::string& text, const glm::mat4& transform,const float& posZ,const float& cameraHeight,const Mathf::vec3& cameraPos, glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f), std::string fontName = "default");//render the text with varying MVP

        //member variables
        std::unique_ptr<Shader> textShader_ptr;

        //TO DO: Change to unique ptrs 
        //std::unique_ptr<VertexArray> textVAO;
        //std::unique_ptr<VertexBuffer> textVBO;
        //std::unique_ptr<IndexBuffer> m_iboidQuad;
        //std::unique_ptr<Shader> m_shaderidQuad;

        unsigned int quadTextVAO;
        unsigned int quadTextVBO;
    };


}


#endif // !FONTRENDER_H
	