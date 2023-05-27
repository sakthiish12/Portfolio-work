// /******************************************************************************/
// /*!
// \file         Shader.h
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         08th Oct, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the declarations of the shader class 
// \lines        36 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <unordered_map> //hash map 
#include "glm/glm.hpp"

/****************************************
Brief - stores the vertex and fragment shader files
****************************************/
struct ShaderProgramSource {
	std::string VertexSource;
	std::string FragmentSource;
};

/****************************************
Brief - Shader class 
****************************************/
class Shader {
private:	
	std::string m_FilePath;
	unsigned int m_RendererID;
	//caching for uniforms 
	std::unordered_map<std::string, int> m_uniformLocationCache; //to store the location of the uniforms to make it much faster

public:
	//creating shader based on the file path 
	Shader(const std::string& filepath);
	~Shader(); //destructor 

	//blinding & unbinding shader 
	void Bind() const; //bind the shader 
	void Unbind() const; //unbind the shader

	//getting the rendererID
	unsigned int GetRendererID();//getter for the renderer ID

	//Set Uniform
	void SetUniform1i(const std::string& name,int value); //set uniform vector 1 integer 
	void SetUniform1f(const std::string& name, float value); //set uniform vector 1 float 
	void SetUniform3f(const std::string& name, float v0, float v1, float v2); //set uniform vector 3 float 
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3); //set uniform vector 4 float 
	void SetUniformMat3f(const std::string& name, const glm::mat3& matrix); //set uniform 3x3 matrix float 
	void SetUniformMat4f(const std::string& name,const glm::mat4& matrix); //set uniform 4x4 matrix float 

	void PrintActiveAttribs() const;// display the list of active vertex attributes used by vertex shader

	void PrintActiveUniforms() const;// display the list of active uniform variables
private:
	ShaderProgramSource Parseshader(const std::string& filepath); //parse the shader into the fragment and vertex shader 
	unsigned int Compileshader(unsigned int type, const std::string& source); //compile the shader 
	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentshader); //create both the vertex and fragment shader 
	int GetUniformLocation(const std::string& name); //get the uniform location

	bool FileExists(std::string const& file_name);// return true if file (given in relative path) exists, false otherwise
};
#endif // !SHADER_H
