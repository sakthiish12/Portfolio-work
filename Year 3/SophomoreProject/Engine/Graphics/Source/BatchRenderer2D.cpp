// /******************************************************************************/
// /*!
// \file         BatchRender2D.cpp
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
#include "BatchRenderer2D.h"
#include "../../WinMgr/WinMgr.h"
#include "texture.h"
#include "VertexBufferLayout.h"
#include "../../Debugging/Log.h"
#include <glm/gtc/matrix_transform.hpp>
#include "../../AssetManager/AssetManager.h"//asset manager 

namespace b_RenderData {

	static BatchRenderer2D::RendererData2D s_RendererData;//static renderer data 
	static unsigned int numofTextures;//number of textures to bind 
}


BatchRenderer2D::BatchRenderer2D() {
	//init the vao 
	m_vaoidQuad = std::make_unique<VertexArray>();

	//init the vbo 
	m_vboidQuad = std::make_unique<VertexBuffer>(static_cast<unsigned int>(RendererData2D::MAX_VERTEX_COUNT)); //for batch rendering make 20000 for now

	//make layout
	VertexBufferLayout layout;
	layout.Push<VertexBuffer::QuadVertex>(static_cast<unsigned int>(RendererData2D::MAX_QUAD_COUNT));//vbo able to store max quad count 

	//setting the vao
	m_vaoidQuad->AddBuffer(*m_vboidQuad, layout);

	std::vector<unsigned int> indicesTotal(static_cast<unsigned int>(RendererData2D::MAX_INDEX_COUNT)); //make it big optimization 

	//create max quad count indices for now -> to do: make it dynamic as well 
	unsigned int index{ 0 };

	//loop to create max quad count 
	for (size_t i{ 0 }; i < RendererData2D::MAX_INDEX_COUNT; i+=6) {
		indicesTotal[i]   = index;
		indicesTotal[i+1] = index + 1;
		indicesTotal[i+2] = index + 2;
		indicesTotal[i+3] = index + 2;
		indicesTotal[i+4] = index + 3;
		indicesTotal[i+5] = index;
		index += 4;//increment by 4 
	}

	//setting the index buffer -> to do: make it dynamic as well 
	m_iboidQuad = std::make_unique<IndexBuffer>(indicesTotal.data(), static_cast<unsigned int>(indicesTotal.size()));

	//base shader TO DO : add more variations of the base shader 
	m_shaderidQuad = std::move(AssetManager::GetInstance().shaderProg.find("NewBatching.shader")->second);

	m_shaderidQuad->Bind();//binding the shader 

	b_RenderData::numofTextures = AssetManager::GetInstance().GetNumSpritesBinded();//getting the number of sprites binded 

	//getting the location of the shader for texture batching 
	auto loc = glGetUniformLocation(m_shaderidQuad->GetRendererID(), "u_Texture");
	int samplers[32] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31 };
	glUniform1iv(loc, 32, samplers);


	//unbind quad 
	m_shaderidQuad->Unbind();
	m_iboidQuad->UnBind();
	m_vboidQuad->UnBind();
	m_vaoidQuad->UnBind();

	//******FOR THE LINE SETUP******///
	
	//init the vao 
	m_vaoidLines = std::make_unique<VertexArray>();

	//init the vbo 
	m_vboidLines = std::make_unique<VertexBuffer>(static_cast<unsigned int>(RendererData2D::MAX_LINEVERTEX_COUNT)); //for batch rendering make 20000 for now

	//make layout
	VertexBufferLayout layoutlines;
	layoutlines.Push<float>(3);//push layout for the position 
	layoutlines.Push<float>(4);//push layout for the color 

	//setting the vao
	m_vaoidLines->AddBuffer(*m_vboidLines, layoutlines);

	std::vector<unsigned int> indicesTotalLines(static_cast<unsigned int>(RendererData2D::MAX_LINEINDEX_COUNT)); //make it big optimization 

	//loop to create max quad count 
	for (unsigned int i{ 0 }; i < RendererData2D::MAX_LINEINDEX_COUNT; i+=2) {
		indicesTotalLines[i] = i;
		indicesTotalLines[i + 1U] = i + 1;
	}

	//setting the index buffer -> to do: make it dynamic as well 
	m_iboidLines = std::make_unique<IndexBuffer>(indicesTotalLines.data(), static_cast<unsigned int>(indicesTotalLines.size()));

	//base shader TO DO : add more variations of the base shader 
	m_shaderidLines = std::move(AssetManager::GetInstance().shaderProg.find("LineBatch.shader")->second);

	m_shaderidLines->Bind();//binding the shader 


	//***********FOR THE QUAD MESH SETUP*************//
	//init the vao 
	m_vaoidQuadMesh = std::make_unique<VertexArray>();

	//init the vbo 
	m_vboidQuadMesh = std::make_unique<VertexBuffer>(static_cast<unsigned int>(RendererData2D::MAX_VERTEX_COUNT)); //for batch rendering make 20000 for now

	//make layout
	VertexBufferLayout layoutQuadMesh;
	layoutQuadMesh.Push<VertexBuffer::QuadVertexMesh>(static_cast<unsigned int>(RendererData2D::MAX_QUAD_COUNT));//vbo able to store max quad count 

	//setting the vao
	m_vaoidQuadMesh->AddBuffer(*m_vboidQuadMesh, layoutQuadMesh);

	std::vector<unsigned int> indicesTotalQuadMesh(static_cast<unsigned int>(RendererData2D::MAX_INDEX_COUNT)); //make it big optimization 

	//create max quad count indices for now -> to do: make it dynamic as well 
	unsigned int indexQuadMesh{ 0 };

	//loop to create max quad count 
	for (size_t i{ 0 }; i < RendererData2D::MAX_INDEX_COUNT; i += 6) {
		indicesTotalQuadMesh[i] = indexQuadMesh;
		indicesTotalQuadMesh[i + 1] = indexQuadMesh + 1;
		indicesTotalQuadMesh[i + 2] = indexQuadMesh + 2;
		indicesTotalQuadMesh[i + 3] = indexQuadMesh + 2;
		indicesTotalQuadMesh[i + 4] = indexQuadMesh + 3;
		indicesTotalQuadMesh[i + 5] = indexQuadMesh;
		indexQuadMesh += 4;//increment by 4 
	}

	//setting the index buffer -> to do: make it dynamic as well 
	m_iboidQuadMesh = std::make_unique<IndexBuffer>(indicesTotalQuadMesh.data(), static_cast<unsigned int>(indicesTotalQuadMesh.size()));

	//base shader TO DO : add more variations of the base shader 
	m_shaderidQuadMesh = std::move(AssetManager::GetInstance().shaderProg.find("QuadMeshBatch.shader")->second);

	m_shaderidQuadMesh->Bind();//binding the shader 

	//unbind quad mesh
	m_shaderidQuadMesh->Unbind();
	m_iboidQuadMesh->UnBind();
	m_vboidQuadMesh->UnBind();
	m_vaoidQuadMesh->UnBind();

	//******FOR THE TRIANGLE SETUP******///

	//init the vao 
	m_vaoidTriangle = std::make_unique<VertexArray>();

	//init the vbo 
	m_vboidTriangle = std::make_unique<VertexBuffer>(static_cast<unsigned int>(RendererData2D::MAX_TRIANGLE_VERTEX_COUNT)); //for batch rendering make 20000 for now

	//make layout
	VertexBufferLayout layoutTriangle;
	layoutTriangle.Push<float>(3);//push layout for the position 
	layoutTriangle.Push<float>(4);//push layout for the color 

	//setting the vao
	m_vaoidTriangle->AddBuffer(*m_vboidTriangle, layoutTriangle);

	std::vector<unsigned int> indicesTotalTriangle(static_cast<unsigned int>(RendererData2D::MAX_TRIANGLE_INDEX_COUNT)); //make it big optimization 

	//loop to create max quad count 
	for (unsigned int i{ 0 }; i < RendererData2D::MAX_TRIANGLE_INDEX_COUNT; i += 3) {
		indicesTotalTriangle[i] = i;
		indicesTotalTriangle[i + 1U] = i + 1;
		indicesTotalTriangle[i + 2U] = i + 2;
	}

	//setting the index buffer -> to do: make it dynamic as well 
	m_iboidTriangle = std::make_unique<IndexBuffer>(indicesTotalTriangle.data(), static_cast<unsigned int>(indicesTotalTriangle.size()));

	//base shader TO DO : add more variations of the base shader 
	m_shaderidTriangle = std::move(AssetManager::GetInstance().shaderProg.find("TriangleBatch.shader")->second);

	m_shaderidTriangle->Bind();//binding the shader 
}


void BatchRenderer2D::AddQuad(const float& x,const float& y, const float& z, const std::string& textureName,const float& scalex,const float& scaley, const Mathf::Vector4& clr,const std::array<Mathf::Vector2, 4>& texCoord) {
		
	float textureID;

	//if it's already in the texture slot vec, get the texture slot 
	std::vector<std::string>::iterator textureSlotVecIter = std::find(b_RenderData::s_RendererData.textureSlotVec.begin(), b_RenderData::s_RendererData.textureSlotVec.end(), textureName);
	if (textureSlotVecIter != b_RenderData::s_RendererData.textureSlotVec.end()) {
		textureID = static_cast<float>(textureSlotVecIter - b_RenderData::s_RendererData.textureSlotVec.begin());//getting the texture id 
	}
	else {
		//add it into the texture slot map 
		b_RenderData::s_RendererData.textureSlotVec.emplace_back(textureName);
		textureID = b_RenderData::s_RendererData.textureSlotIndex;
		b_RenderData::s_RendererData.textureSlotIndex++;//incrementing it 
	}


	//making the vertex 
	VertexBuffer::QuadVertex v0;//bottom left 
	v0.position[0] = { x }, v0.position[1] = { y }, v0.position[2] = { z };
	v0.color[0] = { clr.x }, v0.color[1] = { clr.y }, v0.color[2] = { clr.z }, v0.color[3] = { clr.w };
	v0.texCoords[0] = { texCoord[0].x }, v0.texCoords[1] = { texCoord[0].y };
	v0.texID = { textureID };

	VertexBuffer::QuadVertex v1;//bottom right 
	v1.position[0] = { x + scalex }, v1.position[1] = { y }, v1.position[2] = { z };
	v1.color[0] = { clr.x }, v1.color[1] = { clr.y }, v1.color[2] = { clr.z }, v1.color[3] = { clr.w };
	v1.texCoords[0] = { texCoord[1].x }, v1.texCoords[1] = { texCoord[1].y };
	v1.texID = { textureID };

	VertexBuffer::QuadVertex v2;//top right 
	v2.position[0] = { x + scalex }, v2.position[1] = { y + scaley }, v2.position[2] = { z };
	v2.color[0] = { clr.x }, v2.color[1] = { clr.y }, v2.color[2] = { clr.z }, v2.color[3] = { clr.w };
	v2.texCoords[0] = { texCoord[2].x }, v2.texCoords[1] = { texCoord[2].y };
	v2.texID = { textureID };

	VertexBuffer::QuadVertex v3;//top left 
	v3.position[0] = { x }, v3.position[1] = { y + scaley }, v3.position[2] = { z };
	v3.color[0] = { clr.x }, v3.color[1] = { clr.y }, v3.color[2] = { clr.z }, v3.color[3] = { clr.w };
	v3.texCoords[0] = { texCoord[3].x }, v3.texCoords[1] = { texCoord[3].y };
	v3.texID = { textureID };

	//adding the vertex into the buffer 
	b_RenderData::s_RendererData.buffer.emplace_back(std::move(v0));
	b_RenderData::s_RendererData.buffer.emplace_back(std::move(v1));
	b_RenderData::s_RendererData.buffer.emplace_back(std::move(v2));
	b_RenderData::s_RendererData.buffer.emplace_back(std::move(v3));

	//incre the number of quads drawn 
	numOfQuadsDrawn++;
}

void BatchRenderer2D::Flush() {

	b_RenderData::s_RendererData.buffer.clear();//clearing of the buffer 
	b_RenderData::s_RendererData.quadMeshBuffer.clear();//clearing of the buffer of the quad mesh 
	b_RenderData::s_RendererData.linebuffer.clear();//clearing the line buffer 
	b_RenderData::s_RendererData.triangleMeshBuffer.clear();//clearing the triangle buffer 
	b_RenderData::s_RendererData.textureSlotVec.clear();//clearing the texture slot vector 

	b_RenderData::s_RendererData.textureSlotIndex = 0.f;//reseting the texture slot index 


	if (BatchRenderer2D::numOfLinesDrawn != 0) {
		m_vboidLines->Bind();//binding the vbo lines
		std::vector<VertexBuffer::LineVertex> clearBufferLine(BatchRenderer2D::numOfLinesDrawn * 2);
		m_vboidLines->SubDataLines(clearBufferLine.data(), static_cast<unsigned int>(BatchRenderer2D::numOfLinesDrawn) * 2);//pass in the data into the vbo 
		m_vboidLines->UnBind();//unbind the vbo lines 
		BatchRenderer2D::numOfLinesDrawn = 0;//reset the number of lines drawn 
	}

	if (BatchRenderer2D::numOfQuadsDrawn != 0) {
		m_vboidQuad->Bind();	
		std::vector<VertexBuffer::QuadVertex> clearBufferQuad(BatchRenderer2D::numOfQuadsDrawn * 4); //contains the line Vertex Buffer
		m_vboidQuad->SubData(clearBufferQuad.data(), static_cast<unsigned int>(BatchRenderer2D::numOfQuadsDrawn) * 4);//pass in the data into the vbo 
		m_vboidQuad->UnBind();//unbind the vbo lines 
		BatchRenderer2D::numOfQuadsDrawn = 0;//reset the number of quads drawn 	
	}

	if (BatchRenderer2D::numOfQuadMeshDrawn != 0) {
		m_vboidQuadMesh->Bind();
		std::vector<VertexBuffer::QuadVertex> clearBufferQuadMesh(BatchRenderer2D::numOfQuadMeshDrawn * 4); //contains the line Vertex Buffer
		m_vboidQuadMesh->SubData(clearBufferQuadMesh.data(), static_cast<unsigned int>(BatchRenderer2D::numOfQuadMeshDrawn) * 4);//pass in the data into the vbo 
		m_vboidQuadMesh->UnBind();//unbind the vbo lines 
		BatchRenderer2D::numOfQuadMeshDrawn = 0;//reset the number of quads drawn 	
	}

	if (BatchRenderer2D::numOfTriangleMeshDrawn != 0) {
		m_vboidTriangle->Bind();
		std::vector<VertexBuffer::TriangleVertexMesh> clearBufferTriangleMesh(BatchRenderer2D::numOfTriangleMeshDrawn * 3); //contains the line Vertex Buffer
		m_vboidTriangle->SubDataTriangles(clearBufferTriangleMesh.data(), static_cast<unsigned int>(BatchRenderer2D::numOfTriangleMeshDrawn) * 3);//pass in the data into the vbo 
		m_vboidTriangle->UnBind();//unbind the vbo lines 
		BatchRenderer2D::numOfTriangleMeshDrawn = 0;//reset the number of quads drawn 	
	}
}

void BatchRenderer2D::AddRotatedQuad(const glm::mat4& transform,const float& z,const std::string& textureName,const Mathf::Vector4& clr,const std::array<Mathf::Vector2, 4>& texCoord) {
	
	float textureID;

	//if it's already in the texture slot vec, get the texture slot 
	std::vector<std::string>::iterator textureSlotVecIter = std::find(b_RenderData::s_RendererData.textureSlotVec.begin(), b_RenderData::s_RendererData.textureSlotVec.end(), textureName);
	if (textureSlotVecIter != b_RenderData::s_RendererData.textureSlotVec.end()) {
		textureID = static_cast<float>(textureSlotVecIter - b_RenderData::s_RendererData.textureSlotVec.begin());//getting the texture id 
	}
	else {
		//add it into the texture slot map 
		b_RenderData::s_RendererData.textureSlotVec.emplace_back(textureName);
		textureID = b_RenderData::s_RendererData.textureSlotIndex;
		b_RenderData::s_RendererData.textureSlotIndex++;//incrementing it 
	}
	
	//basic starting pt
	glm::vec3 vecV0 = glm::vec3{ -0.5f, -0.5f, 1.f };
	glm::vec3 vecV1 = glm::vec3{  0.5f, -0.5f, 1.f };
	glm::vec3 vecV2 = glm::vec3{  0.5f,  0.5f, 1.f };
	glm::vec3 vecV3 = glm::vec3{ -0.5f,  0.5f, 1.f };
	
	//get the update position 
	vecV0 = transform * glm::vec4(vecV0,1.0f);
	vecV1 = transform * glm::vec4(vecV1,1.0f);
	vecV2 = transform * glm::vec4(vecV2,1.0f);
	vecV3 = transform * glm::vec4(vecV3,1.0f);

	//making the vertex 
	VertexBuffer::QuadVertex v0;//bottom left 
	v0.position[0] = { vecV0.x }, v0.position[1] = { vecV0.y }, v0.position[2] = { z };
	v0.color[0] = { clr.x }, v0.color[1] = { clr.y }, v0.color[2] = { clr.z }, v0.color[3] = { clr.w };
	v0.texCoords[0] = { texCoord[0].x }, v0.texCoords[1] = { texCoord[0].y };
	v0.texID = { textureID };

	VertexBuffer::QuadVertex v1;//bottom right 
	v1.position[0] = { vecV1.x }, v1.position[1] = { vecV1.y }, v1.position[2] = { z };
	v1.color[0] = { clr.x }, v1.color[1] = { clr.y }, v1.color[2] = { clr.z }, v1.color[3] = { clr.w };
	v1.texCoords[0] = { texCoord[1].x }, v1.texCoords[1] = { texCoord[1].y };
	v1.texID = { textureID };

	VertexBuffer::QuadVertex v2;//top right 
	v2.position[0] = { vecV2.x }, v2.position[1] = { vecV2.y }, v2.position[2] = { z };
	v2.color[0] = { clr.x }, v2.color[1] = { clr.y }, v2.color[2] = { clr.z }, v2.color[3] = { clr.w };
	v2.texCoords[0] = { texCoord[2].x }, v2.texCoords[1] = { texCoord[2].y };
	v2.texID = { textureID };

	VertexBuffer::QuadVertex v3;//top left 
	v3.position[0] = { vecV3.x }, v3.position[1] = { vecV3.y }, v3.position[2] = { z };
	v3.color[0] = { clr.x }, v3.color[1] = { clr.y }, v3.color[2] = { clr.z }, v3.color[3] = { clr.w };
	v3.texCoords[0] = { texCoord[3].x }, v3.texCoords[1] = { texCoord[3].y };
	v3.texID = { textureID };

	//adding the vertex into the buffer 
	b_RenderData::s_RendererData.buffer.emplace_back(std::move(v0));
	b_RenderData::s_RendererData.buffer.emplace_back(std::move(v1));
	b_RenderData::s_RendererData.buffer.emplace_back(std::move(v2));
	b_RenderData::s_RendererData.buffer.emplace_back(std::move(v3));

	//incre the number of quads drawn 
	numOfQuadsDrawn++;

}

void BatchRenderer2D::Draw(const float& cameraHeight,const Mathf::vec3& cameraPosition ) const {
	//bind the buffer id 
	m_vboidQuad->Bind();

	m_vboidQuad->SubData(b_RenderData::s_RendererData.buffer.data(),static_cast<unsigned int>(b_RenderData::s_RendererData.buffer.size()));//pass in the data into the vbo 

	//loop through all the texture slot added get the corresponding texture and binding it 
	for (int i = 0; i < b_RenderData::s_RendererData.textureSlotIndex; ++i) {
		std::string nameOfTexture = b_RenderData::s_RendererData.textureSlotVec[i];//getting the name of the texture 
		glBindTextureUnit(i, (AssetManager::GetInstance().textureMap.find(nameOfTexture)->second)->GetRendererID());//binding the texture to the slot 
	}



	float aspectRatio = (WindowsManager.getWidthF() / WindowsManager.getHeightF());
	float cameraWidth = aspectRatio * cameraHeight;
	m_shaderidQuad->Bind();//bind shader 

	glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(cameraPosition.x, cameraPosition.y, 1.f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(0.f), glm::vec3(0, 0, 1));

	glm::mat4 m_ViewMatrix = glm::inverse(transform);

	//doing up the view projection matrix TO DO: abstract out 
	glm::mat4 viewprojection = glm::ortho(-cameraWidth, cameraWidth, -cameraHeight, cameraHeight, -1.0f, 1.0f) * glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)) * m_ViewMatrix;
	//glm::mat4 viewprojection = glm::ortho(0.0f, cameraWidth, 0.f, cameraHeight, 0.f,0.f) * glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)) * m_ViewMatrix;
	m_shaderidQuad->SetUniformMat4f("u_MVP", viewprojection); //pass into mvp 

	//draw renderer 
	Renderer::GetInstance().sDraw(*m_vaoidQuad, *m_iboidQuad, *m_shaderidQuad);


}

void BatchRenderer2D::CheckOverFlow(const float& cameraHeight,const Mathf::vec3& cameraPostion) {
	//TO DO: if buffer overloaded, draw it first, & clear then resub data 
	if (numOfQuadsDrawn >= BatchRenderer2D::RendererData2D::MAX_QUAD_COUNT || numOfLinesDrawn >= BatchRenderer2D::RendererData2D::MAX_LINE_COUNT 
		|| numOfQuadMeshDrawn >= BatchRenderer2D::RendererData2D::MAX_QUAD_COUNT || numOfTriangleMeshDrawn >= BatchRenderer2D::RendererData2D::MAX_TRIANGLE_COUNT) {
		BatchRenderer2D::Draw(cameraHeight, cameraPostion);//draw quads
		BatchRenderer2D::DrawLine(cameraHeight, cameraPostion);//draw lines
		BatchRenderer2D::DrawTriangle(cameraHeight, cameraPostion);//draw lines
		BatchRenderer2D::DrawQuadMesh(cameraHeight, cameraPostion);//drawing quad mesh 
		BatchRenderer2D::Flush();//flush it
	}
	if (b_RenderData::s_RendererData.textureSlotIndex == b_RenderData::s_RendererData.MAX_TEXTURE_SLOTS) {
		BatchRenderer2D::Draw(cameraHeight, cameraPostion);//draw quads
		BatchRenderer2D::DrawLine(cameraHeight, cameraPostion);//draw lines
		BatchRenderer2D::DrawTriangle(cameraHeight, cameraPostion);//draw lines
		BatchRenderer2D::DrawQuadMesh(cameraHeight, cameraPostion);//drawing quad mesh 
		BatchRenderer2D::Flush();//flush it
	}
	
}


void BatchRenderer2D::DrawLine(const float& cameraHeight, const Mathf::vec3& cameraPosition) const {
	//bind the buffer id 
	m_vboidLines->Bind();

	m_vboidLines->SubDataLines(b_RenderData::s_RendererData.linebuffer.data(), static_cast<unsigned int>(b_RenderData::s_RendererData.linebuffer.size()));//pass in the data into the vbo 

	float aspectRatio = (WindowsManager.getWidthF() / WindowsManager.getHeightF());
	float cameraWidth = aspectRatio * cameraHeight;
	m_shaderidLines->Bind();//bind shader 

	glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(cameraPosition.x, cameraPosition.y, 1.f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(0.f), glm::vec3(0, 0, 1));

	glm::mat4 m_ViewMatrix = glm::inverse(transform);


	glm::mat4 viewprojection = glm::ortho(-cameraWidth, cameraWidth, -cameraHeight, cameraHeight, -1.0f, 1.0f) * glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)) * m_ViewMatrix;
	m_shaderidLines->SetUniformMat4f("u_MVP", viewprojection); //pass into mvp 

	//draw the lines
	Renderer::GetInstance().lineDraw(*m_vaoidLines,*m_iboidLines,*m_shaderidLines);

}


void BatchRenderer2D::AddRotateLine(const float& x0, const float& y0, const float& x1, const float& y1, const float& r, const float& g , const float& b) {

	//making the vertex 
	VertexBuffer::LineVertex v0;//bottom left 
	v0.position[0] = { x0 }, v0.position[1] = { y0 }, v0.position[2] = { 1.f };
	v0.color[0] = { r }, v0.color[1] = { g }, v0.color[2] = { b }, v0.color[3] = { 1.0f };

	VertexBuffer::LineVertex v1;//bottom right 
	v1.position[0] = { x1 }, v1.position[1] = { y1 }, v1.position[2] = { 1.f };
	v1.color[0] = { r }, v1.color[1] = { g }, v1.color[2] = { b }, v1.color[3] = { 1.0f };


	//adding the vertex into the buffer 
	b_RenderData::s_RendererData.linebuffer.emplace_back(std::move(v0));
	b_RenderData::s_RendererData.linebuffer.emplace_back(std::move(v1));


	//incre the number of lines drawn 
	numOfLinesDrawn++;

}


void BatchRenderer2D::AddRotatedQuadMesh(const glm::mat4& transform, const float& z, const Mathf::Vector4& clr) {
	//basic starting pt
	glm::vec3 vecV0 = glm::vec3{ -0.5f, -0.5f, 1.f };
	glm::vec3 vecV1 = glm::vec3{ 0.5f, -0.5f, 1.f };
	glm::vec3 vecV2 = glm::vec3{ 0.5f,  0.5f, 1.f };
	glm::vec3 vecV3 = glm::vec3{ -0.5f,  0.5f, 1.f };

	//get the update position 
	vecV0 = transform * glm::vec4(vecV0, 1.0f);
	vecV1 = transform * glm::vec4(vecV1, 1.0f);
	vecV2 = transform * glm::vec4(vecV2, 1.0f);
	vecV3 = transform * glm::vec4(vecV3, 1.0f);

	//making the vertex 
	VertexBuffer::QuadVertexMesh v0;//bottom left 
	v0.position[0] = { vecV0.x }, v0.position[1] = { vecV0.y }, v0.position[2] = { z };
	v0.color[0] = { clr.x }, v0.color[1] = { clr.y }, v0.color[2] = { clr.z }, v0.color[3] = { clr.w };

	VertexBuffer::QuadVertexMesh v1;//bottom right 
	v1.position[0] = { vecV1.x }, v1.position[1] = { vecV1.y }, v1.position[2] = { z };
	v1.color[0] = { clr.x }, v1.color[1] = { clr.y }, v1.color[2] = { clr.z }, v1.color[3] = { clr.w };

	VertexBuffer::QuadVertexMesh v2;//top right 
	v2.position[0] = { vecV2.x }, v2.position[1] = { vecV2.y }, v2.position[2] = { z };
	v2.color[0] = { clr.x }, v2.color[1] = { clr.y }, v2.color[2] = { clr.z }, v2.color[3] = { clr.w };

	VertexBuffer::QuadVertexMesh v3;//top left 
	v3.position[0] = { vecV3.x }, v3.position[1] = { vecV3.y }, v3.position[2] = { z };
	v3.color[0] = { clr.x }, v3.color[1] = { clr.y }, v3.color[2] = { clr.z }, v3.color[3] = { clr.w };

	//adding the vertex into the buffer 
	b_RenderData::s_RendererData.quadMeshBuffer.emplace_back(std::move(v0));
	b_RenderData::s_RendererData.quadMeshBuffer.emplace_back(std::move(v1));
	b_RenderData::s_RendererData.quadMeshBuffer.emplace_back(std::move(v2));
	b_RenderData::s_RendererData.quadMeshBuffer.emplace_back(std::move(v3));

	//incre the number of quads drawn 
	numOfQuadMeshDrawn++;
}


void BatchRenderer2D::DrawQuadMesh(const float& cameraHeight, const Mathf::vec3& cameraPosition) const {
	//bind the buffer id 
	m_vboidQuadMesh->Bind();

	m_vboidQuadMesh->SubDataQuadMesh(b_RenderData::s_RendererData.quadMeshBuffer.data(), static_cast<unsigned int>(b_RenderData::s_RendererData.quadMeshBuffer.size()));//pass in the data into the vbo 

	float aspectRatio = (WindowsManager.getWidthF() / WindowsManager.getHeightF());
	float cameraWidth = aspectRatio * cameraHeight;
	m_shaderidQuadMesh->Bind();//bind shader 

	glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(cameraPosition.x, cameraPosition.y, 1.f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(0.f), glm::vec3(0, 0, 1));

	glm::mat4 m_ViewMatrix = glm::inverse(transform);

	//doing up the view projection matrix TO DO: abstract out 
	glm::mat4 viewprojection = glm::ortho(-cameraWidth, cameraWidth, -cameraHeight, cameraHeight, -1.0f, 1.0f) * glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)) * m_ViewMatrix;
	//glm::mat4 viewprojection = glm::ortho(0.0f, cameraWidth, 0.f, cameraHeight, 0.f,0.f) * glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)) * m_ViewMatrix;
	m_shaderidQuadMesh->SetUniformMat4f("u_MVP", viewprojection); //pass into mvp 

	//draw renderer 
	Renderer::GetInstance().sDraw(*m_vaoidQuadMesh, *m_iboidQuadMesh, *m_shaderidQuadMesh);


}


void BatchRenderer2D::DrawTriangle(const float& cameraHeight, const Mathf::vec3& cameraPosition) const {
	//bind the buffer id 
	m_vboidTriangle->Bind();

	m_vboidTriangle->SubDataTriangles(b_RenderData::s_RendererData.triangleMeshBuffer.data(), static_cast<unsigned int>(b_RenderData::s_RendererData.triangleMeshBuffer.size()));//pass in the data into the vbo 

	float aspectRatio = (WindowsManager.getWidthF() / WindowsManager.getHeightF());
	float cameraWidth = aspectRatio * cameraHeight;
	m_shaderidTriangle->Bind();//bind shader 

	glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(cameraPosition.x, cameraPosition.y, 1.f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(0.f), glm::vec3(0, 0, 1));

	glm::mat4 m_ViewMatrix = glm::inverse(transform);


	glm::mat4 viewprojection = glm::ortho(-cameraWidth, cameraWidth, -cameraHeight, cameraHeight, -1.0f, 1.0f) * glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)) * m_ViewMatrix;
	m_shaderidTriangle->SetUniformMat4f("u_MVP", viewprojection); //pass into mvp 

	//draw the lines
	Renderer::GetInstance().sDrawTriangle(*m_vaoidTriangle, *m_iboidTriangle, *m_shaderidTriangle);

}

void BatchRenderer2D::AddRotateTriangle(const float& x0, const float& y0, const float& x1, const float& y1, const float& x2, const float& y2,const float& r, const float& g, const float& b) {

	//making the vertex 
	VertexBuffer::TriangleVertexMesh v0;//bottom left 
	v0.position[0] = { x0 }, v0.position[1] = { y0 }, v0.position[2] = { 1.f };
	v0.color[0] = { r }, v0.color[1] = { g }, v0.color[2] = { b }, v0.color[3] = { 1.0f };

	VertexBuffer::TriangleVertexMesh v1;//bottom right 
	v1.position[0] = { x1 }, v1.position[1] = { y1 }, v1.position[2] = { 1.f };
	v1.color[0] = { r }, v1.color[1] = { g }, v1.color[2] = { b }, v1.color[3] = { 1.0f };	
	
	VertexBuffer::TriangleVertexMesh v2;//top
	v2.position[0] = { x2 }, v2.position[1] = { y2 }, v2.position[2] = { 1.f };
	v2.color[0] = { r }, v2.color[1] = { g }, v2.color[2] = { b }, v2.color[3] = { 1.0f };


	//adding the vertex into the buffer 
	b_RenderData::s_RendererData.triangleMeshBuffer.emplace_back(std::move(v0));
	b_RenderData::s_RendererData.triangleMeshBuffer.emplace_back(std::move(v1));
	b_RenderData::s_RendererData.triangleMeshBuffer.emplace_back(std::move(v2));


	//incre the number of lines drawn 
	numOfTriangleMeshDrawn++;

}