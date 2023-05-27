// /******************************************************************************/
// /*!
// \file         FontRendering.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         12th Nov, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definitions for the font rendering
// \lines        123 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/

#include <iostream>
#include <FontRendering.h>
#include "texture.h"//texture

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../../WinMgr/WinMgr.h"

using namespace TextRendering;

TextRenderer::TextRenderer(const std::string& ShaderFilePath)
{
    //init freetype 
    if (FT_Init_FreeType(&this->ft)) {
        fprintf(stderr, "Could not init freetype library\n");
    }
    

    textShader_ptr = std::make_unique<Shader>(ShaderFilePath); //making a unique ptr to the shader 
    textShader_ptr->Bind(); //use shader 
    glm::mat4 viewprojection = glm::ortho(0.0f, WindowsManager.getWidthF(), 0.0f, WindowsManager.getHeightF(), -1.0f, 1.0f);
    textShader_ptr->SetUniformMat4f("projection", viewprojection); //pass into the projection
}

TextRenderer::~TextRenderer()
{

    //Loop through both hashmap to free memory
    for (std::map<std::string, Text*>::iterator it = textMap.begin(); it != textMap.end(); it++)
    {
        //delete all the memory for the characters
        for (std::map<char, Character*>::iterator it2 = it->second->characterMap.begin(); it2 != it->second->characterMap.end(); ++it2)
        {
            delete it2->second;
        }
        it->second->characterMap.clear();

        delete it->second;
    }

    textMap.clear();//clearing map 
    FT_Done_FreeType(this->ft);//free 

}

void TextRenderer::InitFont(std::string filePath, std::string fontName)
{
    FT_Face* face_ptr = new FT_Face();

    //init font face
    if (FT_New_Face(this->ft, filePath.c_str(), 0, face_ptr))
    {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return;
    }

    //To Do: Set '48' to be variable instead of constant
    FT_Set_Pixel_Sizes(*face_ptr, 0, 48);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    Text* text_ptr = new Text();

    // load first 128 characters of ASCII set
    for (unsigned char c = 0; c < 128; c++)
    {
        // Load character glyph 
        if (FT_Load_Char(*face_ptr, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            (*face_ptr)->glyph->bitmap.width,
            (*face_ptr)->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            (*face_ptr)->glyph->bitmap.buffer
        );
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        //To Do: possibly change the AA to be GL_NEAREST later
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use
        Character* character_ptr = new Character();
        character_ptr->TextureID = texture;

        character_ptr->Size = glm::ivec2(
            (*face_ptr)->glyph->bitmap.width,
            (*face_ptr)->glyph->bitmap.rows);

        character_ptr->Bearing = glm::ivec2(
            (*face_ptr)->glyph->bitmap_left,
            (*face_ptr)->glyph->bitmap_top);

        character_ptr->Advance = static_cast<unsigned int>(
            (*face_ptr)->glyph->advance.x);


        text_ptr->characterMap.insert(std::pair<char, Character*>
            (c, character_ptr));
    }
    delete face_ptr;
    textMap.insert(std::pair<std::string, Text*>
        (fontName, text_ptr));


    // configure VAO/VBO for texture quads
    // -----------------------------------
    glGenVertexArrays(1, &quadTextVAO);
    glGenBuffers(1, &quadTextVBO);
    glBindVertexArray(quadTextVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadTextVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void TextRenderer::RenderText(std::string text, glm::vec2 pos, float scale, glm::vec3 color, std::string fontName)
{
    // activate corresponding render state	
    textShader_ptr->Bind();

    glUniform3f(glGetUniformLocation(textShader_ptr->GetRendererID(), "textColor"), color.x, color.y, color.z);

    textShader_ptr->SetUniform3f("textColor", color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(quadTextVAO);

    float x = pos.x;
    float y = pos.y;

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = *((textMap.find(fontName)->second)->characterMap[*c]);

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;

        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };

        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, quadTextVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);

        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }

    //clear 
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}


void TextRenderer::RenderTextMVP(std::string text, glm::vec2 pos, float scale,const float& cameraHeight, const Mathf::Vec3& cameraPos, glm::vec3 color,std::string fontName)
{
    // activate corresponding render state	
    textShader_ptr->Bind();

    glUniform3f(glGetUniformLocation(textShader_ptr->GetRendererID(), "textColor"), color.x, color.y, color.z);

    textShader_ptr->SetUniform3f("textColor", color.x, color.y, color.z);

    float aspectRatio = (WindowsManager.getWidthF() / WindowsManager.getHeightF());
    float cameraWidth = aspectRatio * cameraHeight;

    glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(cameraPos.x, cameraPos.y, 1.f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(0.f), glm::vec3(0, 0, 1));

    glm::mat4 m_ViewMatrix = glm::inverse(transform);

    glm::mat4 viewprojection = glm::ortho(-cameraWidth, cameraWidth, -cameraHeight, cameraHeight, -1.0f, 1.0f) * glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)) * m_ViewMatrix;
    textShader_ptr->SetUniformMat4f("projection", viewprojection); //pass into mvp 
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(quadTextVAO);

    float x = pos.x;
    float y = pos.y;

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = *((textMap.find(fontName)->second)->characterMap[*c]);

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;

        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };

        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, quadTextVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);

        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }

    //clear 
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void TextRenderer::RenderTextMVPMatrix(const std::string& text, const glm::mat4& transform2, const float& posZ, const float& cameraHeight, const Mathf::Vec3& cameraPos, glm::vec3 color, std::string fontName)
{
    UNREFERENCED_PARAMETER(posZ);

    // activate corresponding render state	
    textShader_ptr->Bind();

    glUniform3f(glGetUniformLocation(textShader_ptr->GetRendererID(), "textColor"), color.x, color.y, color.z);

    textShader_ptr->SetUniform3f("textColor", color.x, color.y, color.z);

    float aspectRatio = (WindowsManager.getWidthF() / WindowsManager.getHeightF());
    float cameraWidth = aspectRatio * cameraHeight;

    glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(cameraPos.x, cameraPos.y, 1.f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(0.f), glm::vec3(0, 0, 1));

    glm::mat4 m_ViewMatrix = glm::inverse(transform);

    glm::mat4 viewprojection = glm::ortho(-cameraWidth, cameraWidth, -cameraHeight, cameraHeight, -1.0f, 1.0f) * glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)) * m_ViewMatrix;
    textShader_ptr->SetUniformMat4f("projection", viewprojection); //pass into mvp 
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(quadTextVAO);

    float x = transform2[3][0];
    float y = transform2[3][1];

    float scaleX = transform2[0][0];
    float scaleY = transform2[1][1];

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = *((textMap.find(fontName)->second)->characterMap[*c]);

        float xpos = x + ch.Bearing.x * scaleX;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scaleY;

        float w = ch.Size.x * scaleX;
        float h = ch.Size.y * scaleY;

        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };

        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, quadTextVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);

        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scaleX; // bitshift by 6 to get value in pixels (2^6 = 64)
    }

    //clear 
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}