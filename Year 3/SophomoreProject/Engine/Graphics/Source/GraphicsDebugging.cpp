// /******************************************************************************/
// /*!
// \file         GraphicsDebugging.cpp
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
#include "GraphicsDebugging.h"
#include "VertexBufferLayout.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../../WinMgr/WinMgr.h"//windows manager

GraphicsDebugging::GraphicsDebugging() : linePositions{ -0.5f,-0.5f, 0.5f, 0.5f }{

	//loading the meshes 
	AssetManager::GetInstance().LoadMeshes();
	vboxMesh = ((AssetManager::GetInstance().GetMeshes())["square"])[0];
	iboxMesh = ((AssetManager::GetInstance().GetMeshes())["square"])[1];
	//passing in the indices 
	for (size_t i{ 0 }; i < iboxMesh.size(); ++i) {
		boxIndices[i] = static_cast<int>(iboxMesh[i]);
	}

	
	//******make vao for each of the mesh********//

	//mesh for square
	squareVAO = std::make_unique<VertexArray>();
	VertexBuffer vb(vboxMesh.data(), static_cast<unsigned int>(vboxMesh.size() * sizeof(float)));
	squareIBO = std::make_unique<IndexBuffer>(boxIndices, 5); //SOMETHING SEEMS TO CHANGE THE ORDER OF VERTEX SUCH THAT 4 is not enough????? cant rmb
	VertexBufferLayout layout;
	layout.Push<float>(2);
	squareVAO->AddBuffer(vb, layout); //adding the layout of the 



	//mesh for circle


	//mesh for line 
	lineVAO = std::make_unique<VertexArray>();
	VertexBuffer lineVBO(linePositions.data(), static_cast<unsigned int>(linePositions.size() * sizeof(float)));
	VertexBufferLayout layoutline;
	layoutline.Push<float>(2);
	lineVAO->AddBuffer(lineVBO, layoutline);


	//all using the same shader 
	debugShader = std::move(AssetManager::GetInstance().shaderProg.find("debug.shader")->second);

}


GraphicsDebugging::~GraphicsDebugging(){}

//drawing the line 
void GraphicsDebugging::drawLine(const Mathf::Vector2& pt0, const Mathf::Vector2& pt1,const Mathf::Vector4& clr,const float& lineWidth) {
	//getting the pos 
	std::vector<float>pos{ pt0.x,pt0.y,pt1.x,pt1.y };
	
	//calculating view & projection matrix 
	glm::mat4 m_Proj = glm::ortho(0.0f, WindowsManager.getWidthF(), 0.0f, WindowsManager.getHeightF(), -1.0f, 1.0f);
	glm::mat4 m_View = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
	m_View = glm::inverse(m_View);//inverse the view matrix 
	glm::mat4 mvp = m_Proj * m_View;//calculating the model view projection matrix 

	//setting up the buffers and layouts
	VertexArray va;
	VertexBuffer vb(pos.data(), static_cast<unsigned int>(pos.size() * sizeof(float)));

	VertexBufferLayout layout;
	layout.Push<float>(2);

	va.AddBuffer(vb, layout); //adding the layout of the 

	debugShader->Bind();//binding the shader 
	debugShader->SetUniformMat4f("u_MVP", mvp); //pass into mvp 

	debugShader->SetUniform4f("u_Color", clr.x,clr.y,clr.z,clr.w); //r,g,b,a float 

	glLineWidth(lineWidth); //width of the line 
	glVertexAttrib3f(static_cast<GLuint>(clr.x), clr.y, clr.z, clr.w); // color for the lines
	glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(pos.size()));//draw twice 

}


//drawing the filled box with color 
void GraphicsDebugging::drawFilledSquare(const Mathf::Vector2& center, float radius, const Mathf::Vector4& clr){
	Renderer::GetInstance().Clear();
		
	//calculating mvp 
	glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(center.x, center.y,0)); //translation matrix 
	glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(radius*2)); //scale matrix to get the diameter of the box  

	//model matrix
	glm::mat4 model = translation * scale;

	//final model transformation 
	glm::mat4 modelfinal = glm::rotate(model, 0.f, glm::vec3(0, 0, 1)); // where x, y, z is axis of rotation (e.g. 0 0 1))
	glm::mat4 m_Proj = glm::ortho(0.0f, WindowsManager.getWidthF(), 0.0f, WindowsManager.getHeightF(), -1.0f, 1.0f);
	glm::mat4 m_View = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
	glm::mat4 mvp = m_Proj * m_View * modelfinal;//calculating the model view projection matrix 

	//setting up the buffers and layouts
	VertexArray va;
	VertexBuffer vb(vboxMesh.data(), static_cast<unsigned int>(vboxMesh.size() * sizeof(float)));
	IndexBuffer ib(boxIndices, 6);

	VertexBufferLayout layout;
	layout.Push<float>(2);

	va.AddBuffer(vb, layout); //adding the layout of the 

	debugShader->Bind();//bindign the shader 
	debugShader->SetUniformMat4f("u_MVP", mvp); //pass into mvp 

	debugShader->SetUniform4f("u_Color", clr.x,clr.y,clr.z,clr.w); //r,g,b,a float 

	Renderer::GetInstance().sDraw(va, ib, *debugShader);//render box 

	SwapBuffers(WindowsManager.getWindowDC());
	
}

void GraphicsDebugging::drawFilledRect(const Mathf::Vector2& center, const Mathf::Vector2& sca, const Mathf::Vector4& clr)
{
	glm::mat4 mvp = calculateMVP(center, sca);//calculating the model view projection matrix

	//setting up the buffers and layouts
	VertexArray va;
	VertexBuffer vb(vboxMesh.data(),static_cast<unsigned int>(vboxMesh.size() * sizeof(float)));
	IndexBuffer ib(boxIndices, 6);

	VertexBufferLayout layout;
	layout.Push<float>(2);

	va.AddBuffer(vb, layout); //adding the layout of the 

	debugShader->Bind();//bindign the shader 
	debugShader->SetUniformMat4f("u_MVP", mvp); //pass into mvp 
	debugShader->SetUniform4f("u_Color", clr.x, clr.y, clr.z, clr.w); //r,g,b,a float 

	Renderer::GetInstance().sDraw(va, ib, *debugShader);//render box 

	SwapBuffers(WindowsManager.getWindowDC());

}

//getting the instance of the class 
GraphicsDebugging& GraphicsDebugging::GetInstance() {
	//getting the instance of the class
	static GraphicsDebugging instance;
	return instance;
}

glm::mat4 GraphicsDebugging::calculateMVP(const Mathf::Vector2& center, const Mathf::Vector2& sca)
{
	//calculating mvp 
	glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(center.x, center.y, 0)); //translation matrix 
	//glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(radius * 2)); //scale matrix to get the diameter of the box  
	glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(sca.x, sca.y, 0.f)); //scale matrix to get the diameter of the box  
	//model matrix
	glm::mat4 model = translation * scale;
	//final model transformation 
	glm::mat4 modelfinal = glm::rotate(model, 0.f, glm::vec3(0, 0, 1)); // where x, y, z is axis of rotation (e.g. 0 0 1))
	glm::mat4 m_Proj = glm::ortho(0.0f, WindowsManager.getWidthF(), 0.0f, WindowsManager.getHeightF(), -1.0f, 1.0f);
	glm::mat4 m_View = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
	glm::mat4 mvp = m_Proj * m_View * modelfinal;//calculating the model view projection matrix 

	return mvp;
}

//draws a line box 
void GraphicsDebugging::drawLineSquare(const Mathf::Vector2& center, float radius, const Mathf::Vector4& clr) {
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //set to Line mode

	//calculating mvp 
	glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(center.x, center.y, 0)); //translation matrix 
	glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(radius * 2)); //scale matrix to get the diameter of the box  
	//model matrix
	glm::mat4 model = translation * scale;
	//final model transformation 
	glm::mat4 modelfinal = glm::rotate(model, 0.f, glm::vec3(0, 0, 1)); // where x, y, z is axis of rotation (e.g. 0 0 1))
	glm::mat4 m_Proj = glm::ortho(0.0f, WindowsManager.getWidthF(), 0.0f, WindowsManager.getHeightF(), -1.0f, 1.0f);
	glm::mat4 m_View = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
	glm::mat4 mvp = m_Proj * m_View * modelfinal;//calculating the model view projection matrix 

	//setting up the buffers and layouts
	VertexArray va;
	VertexBuffer vb(vboxMesh.data(), static_cast<unsigned int>(vboxMesh.size() * sizeof(float)));
	IndexBuffer ib(boxIndices, 6);

	VertexBufferLayout layout;
	layout.Push<float>(2);

	va.AddBuffer(vb, layout); //adding the layout of the 

	debugShader->Bind();//bindign the shader 
	debugShader->SetUniformMat4f("u_MVP", mvp); //pass into mvp 
	debugShader->SetUniform4f("u_Color", clr.x, clr.y, clr.z, clr.w); //r,g,b,a float 

	Renderer::GetInstance().sDraw(va, ib, *debugShader);//draw 
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //reset to fill mode 
}

void GraphicsDebugging::drawLineRect(const Mathf::Vector2& center, const Mathf::Vector2& sca, const Mathf::Vector4& clr)
{
	
	glm::mat4 mvp = calculateMVP(center, sca);//calculating the model view projection matrix 
	VertexArray va;
	VertexBuffer vb(vboxMesh.data(), 8 * sizeof(float));
	IndexBuffer ib(boxIndices, 5); //SOMETHING SEEMS TO CHANGE THE ORDER OF VERTEX SUCH THAT 4 is not enough????? cant rmb

	VertexBufferLayout layout;
	layout.Push<float>(2);

	va.AddBuffer(vb, layout); //adding the layout of the 
	debugShader->Bind();//bindign the shader 
	debugShader->SetUniformMat4f("u_MVP", mvp); //pass into mvp 

	debugShader->SetUniform4f("u_Color", clr.x, clr.y, clr.z, clr.w); //r,g,b,a float 
	
	//calling out seperatly coz not using GL_TRIANGLE
	debugShader->Bind(); // telling the gpu what to actually do 
//vertex array bind 
	va.Bind(); //the array that contains all the actual data , vertex buffer and index buffer 
	//index buffer bind 
	ib.Bind(); //contains the indices 
	GLCall(glDrawElements(GL_LINE_LOOP, ib.GetCount(), GL_UNSIGNED_INT, nullptr));


	GLCall(glDrawElements(GL_LINE_LOOP, ib.GetCount(), GL_UNSIGNED_INT, nullptr));


}


//draws a filled circle 
void GraphicsDebugging::drawFilledCircle(const Mathf::Vector2& center, const float& radius, const Mathf::Vector4& clr) {
	static_cast<void>(radius);//unreferenced param

	// Generate the (slices+2) count of vertices required to
	// render a triangle fan parameterization of a circle with unit
	// radius and centered at (0, 0)}
	const int circleslices = 50;
	float radian = (2.0f * static_cast<float>(Mathf::PI)) / circleslices;
	std::vector<Mathf::Vector2> pos(circleslices + 2);

	int index = 0;
	pos[index++] = Mathf::Vector2(center.x,center.y);//starting position of the circle 
	//getting the position of the vrtx
	for (int i{ 0 }; i <= circleslices; ++i) {
		float x{ cosf((radian * i)) };
		float y{ sinf(radian * i) };
		//adding the position into the vector 
		pos[index++] = Mathf::Vector2(x, y);
	}

	//changing to the ndc coords 

	//setting up the buffers and layouts
	VertexArray va;
	VertexBuffer vb(pos.data(), static_cast<unsigned int>(pos.size() * sizeof(float)));

	VertexBufferLayout layout;
	layout.Push<float>(2);

	va.AddBuffer(vb, layout); //adding the layout of the 

	
	debugShader->Bind();//bindign the shader 

	debugShader->SetUniform4f("u_Color", clr.x, clr.y, clr.z, clr.w); //r,g,b,a float 

	glDrawArrays(GL_TRIANGLE_FAN, 0, static_cast<GLsizei>(pos.size())); //drawing of the triangle fan


}


void GraphicsDebugging::drawPoint(const Mathf::Vector2& pt0, const float& size, const Mathf::Vector4& clr) {
	//getting the pos 
	std::vector<float>pos{ pt0.x,pt0.y };

	//calculating view & projection matrix 
	glm::mat4 m_Proj = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f);
	glm::mat4 m_View = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
	m_View = glm::inverse(m_View);//inverse the view matrix 
	glm::mat4 mvp = m_Proj * m_View;//calculating the model view projection matrix 

	//setting up the buffers and layouts
	VertexArray va;
	VertexBuffer vb(pos.data(),static_cast<unsigned int>(pos.size() * sizeof(float)));

	VertexBufferLayout layout;
	layout.Push<float>(2);

	va.AddBuffer(vb, layout); //adding the layout of the 

	debugShader->Bind();//bindign the shader 

	debugShader->SetUniformMat4f("u_MVP", mvp); //pass into mvp 
	debugShader->SetUniform4f("u_Color", clr.x, clr.y, clr.z, clr.w); //r,g,b,a float 

	glPointSize(size);//setting the size of the point 
	glVertexAttrib3f(static_cast<GLuint>(clr.x), clr.y, clr.z, clr.w); // color for the lines
	glDrawArrays(GL_POINTS, 0, 1);//draw once 
}