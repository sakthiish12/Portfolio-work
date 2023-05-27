// /******************************************************************************/
// /*!
// \file         Shader.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         08th Oct, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the definitions of the shader class manages the vertex, fragment shader and shader program 
// \lines        144 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include <iostream>
#include <fstream>
#include <sstream>

#include "Shader.h"
#include "Renderer.h"

/****************************************
Brief - Creates the shader by first parsing the shader from the file, getting the file path of the fragment and the vertex shader,
and create the shader returning the rendererID 
****************************************/
Shader::Shader(const std::string& filepath) : m_FilePath(filepath), m_RendererID(0) {

    ShaderProgramSource source = Parseshader(filepath); //relative to the project directory 

    m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);

}

/****************************************
Brief - Delete the shader
****************************************/
Shader::~Shader() {
    GLCall(glDeleteProgram((m_RendererID)));
}

/****************************************
Brief - Parse the vertex and fragment shader 
****************************************/
ShaderProgramSource Shader::Parseshader(const std::string& filepath) {
    std::ifstream stream(filepath);//opens the file 

    //mode
    enum class ShaderType {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];

    //set shader type 
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line)) {
        if (line.find("#shader") != std::string::npos) {
            //find which type of shader it is 
            if (line.find("vertex") != std::string::npos) {
                //set mode to vertex 
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos) {
                //set mode to fragment
                type = ShaderType::FRAGMENT;
            }
        }
        else {
            //any other line of code using arrays if it's the vertex type, add it into the vertex stream, if it's the fragment type, add it into the fragment stream 
            ss[(int)type] << line << "\n";
        }
    }

    return { ss[0].str(), ss[1].str() }; //returning the shaderprogram soure 
}

//compile shader code 
unsigned int Shader::Compileshader(unsigned int type, const std::string& source) {
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str(); //source and string needs to exist else it's going to point to rubberish memory 
    glShaderSource(id, 1, &src, nullptr); //getting the gl shader source 
    glCompileShader(id); //compile the shader with the id 

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        //does not compile succesfully 
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        //allocate on the stack
        char* message = (char*)_malloca(length * sizeof(char)); //allocate on the stack dynamically 
        glGetShaderInfoLog(id, length, &length, message);
        //print out to console 
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragement") << "shader!" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id); //delete shader 
        return 0;
    }


    //TODO: Error Handling 
    return id;
}

//creating a shader code 
unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentshader) {

    /*The glCreateProgram function creates a program and returns the ID reference to the newly created program object. Now we 
    need to attach the previously compiled shaders to the program object and then link them with glLinkProgram:
    */
    //returns an unsigned int 
    unsigned int program = glCreateProgram(); //GLUint is less preferred 
    unsigned int vs = Compileshader(GL_VERTEX_SHADER, vertexShader); // compile shader (vertex shader)
    unsigned int fs = Compileshader(GL_FRAGMENT_SHADER, fragmentshader); // fragment shader 

    //performs validation on the program
    GLCall(glAttachShader(program, vs));
    GLCall(glAttachShader(program, fs));
    GLCall(glLinkProgram(program));
    GLCall(glValidateProgram(program));

    //delete shader
    GLCall(glDeleteShader(vs));
    GLCall(glDeleteShader(fs));

    //return program
    return program;
}

void Shader::Bind() const {
    GLCall((glUseProgram(m_RendererID)));
}

void Shader::Unbind() const {
    GLCall((glUseProgram(0)));
}

/*The uniform is currently empty; we haven't added any data to the uniform yet so let's try that. We first need to 
find the index/location of the uniform attribute in our shader. Once we have the index/location of the uniform, we can update its values. 
Instead of passing a single color to the fragment shader, let's spice things up by gradually changing color over time:*/
void Shader::SetUniform1i(const std::string& name, int value) {
    GLCall(glUniform1i(GetUniformLocation(name),value));
}

void Shader::SetUniform1f(const std::string& name, float value) {
    GLCall(glUniform1f(GetUniformLocation(name), value));
}

void Shader::SetUniform3f(const std::string& name, float v0, float v1, float v2) {
    GLCall(glUniform3f(GetUniformLocation(name), v0, v1, v2));
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3) {
    GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}

void Shader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix) {
    GLCall(glUniformMatrix4fv(GetUniformLocation(name),1, GL_FALSE, &matrix[0][0]));
}

void Shader::SetUniformMat3f(const std::string& name, const glm::mat3& matrix) {
    GLCall(glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
}

int Shader::GetUniformLocation(const std::string& name) {

    //performance boost 
    if (m_uniformLocationCache.find(name) != m_uniformLocationCache.end()) {
        return m_uniformLocationCache[name];
    }

    //when dealing with uniformlocation the type should be a 32 bit int not unsigned int 
    GLCall(int location = glGetUniformLocation(m_RendererID, name.c_str()));
    if (location == -1) {
        std::cout << "Warning : Uniform " << name << " does not exist! " << std::endl;
    }

    //performance boost 
    m_uniformLocationCache[name] = location;

    return location;
}


/****************************************
Brief - Printing the active attributes 
****************************************/
void Shader::PrintActiveAttribs() const {
    //printing the active attritbutes 
    GLint max_length, num_attribs;
    glGetProgramiv(m_RendererID, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &max_length);
    glGetProgramiv(m_RendererID, GL_ACTIVE_ATTRIBUTES, &num_attribs);
    GLchar* pname = new GLchar[max_length];
    std::cout << "Index\t|\tName\n";
    std::cout << "----------------------------------------------------------------------\n";
    for (GLint i = 0; i < num_attribs; ++i) {
        GLsizei written;
        GLint size;
        GLenum type;
        glGetActiveAttrib(m_RendererID, i, max_length, &written, &size, &type, pname);
        GLint loc = glGetAttribLocation(m_RendererID, pname);
        std::cout << loc << "\t\t" << pname << std::endl;
    }
    std::cout << "----------------------------------------------------------------------\n";
    delete[] pname;
}

/****************************************
Brief - Printing the active uniforms 
****************************************/
void Shader::PrintActiveUniforms() const {
    GLint max_length;
    glGetProgramiv(m_RendererID, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_length);
    GLchar* pname = new GLchar[max_length];
    GLint num_uniforms;
    glGetProgramiv(m_RendererID, GL_ACTIVE_UNIFORMS, &num_uniforms);
    std::cout << "Location\t|\tName\n";
    std::cout << "----------------------------------------------------------------------\n";
    for (GLint i = 0; i < num_uniforms; ++i) {
        GLsizei written;
        GLint size;
        GLenum type;
        glGetActiveUniform(m_RendererID, i, max_length, &written, &size, &type, pname);
        GLint loc = glGetUniformLocation(m_RendererID, pname);
        std::cout << loc << "\t\t" << pname << std::endl;
    }
    std::cout << "----------------------------------------------------------------------\n";
    delete[] pname;
}

/*************************
* @Brief - validates if the shader file exist : return true if file (given in relative path) exists, false otherwise
***************************/
bool Shader::FileExists(std::string const& file_name) {
    std::ifstream infile(file_name); return infile.good();
}


//for getting the renderer id 
unsigned int Shader:: GetRendererID() {
    return m_RendererID;
}