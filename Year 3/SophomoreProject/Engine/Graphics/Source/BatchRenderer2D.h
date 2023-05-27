// /******************************************************************************/
// /*!
// \file         BatchRenderer2D.h
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
#ifndef  BATCHRENDERER2D_H
#define  BATCHRENDERER2D_H

#include <memory>
#include <array>
#include "../../Math/Vector/Vector2.h"
//#include "../../pch.h"

#include "../../Graphics/Source/GraphicsApplication.h" //for the glm
#include "../../Graphics/Source/Renderer.h"//renderer 
#include "../../Math/Vector/Vector3.h"//vec3 
#include "../../Math/Vector/Vector4.h"//vec4

class BatchRenderer2D {

public:

	//Basic Renderer Data
	struct RendererData2D
	{
		GLuint defaultTexture = 0; //default texture slot 

		std::vector<VertexBuffer::QuadVertex> buffer; //contains the Quad Vertex Buffer
		std::vector<VertexBuffer::LineVertex> linebuffer; //contains the line Vertex Buffer
		std::vector<VertexBuffer::QuadVertexMesh> quadMeshBuffer;//contains the quad mesh vertex buffer
		std::vector<VertexBuffer::TriangleVertexMesh> triangleMeshBuffer;//contains the quad mesh vertex buffer

		std::vector<std::string> textureSlotVec; //stores the current texture slots map 
		float textureSlotIndex = 0;//stores the texture slot index

		//const static data
		static const size_t MAX_QUAD_COUNT = 3000;
		static const size_t MAX_VERTEX_COUNT = MAX_QUAD_COUNT * 4;
		static const size_t MAX_INDEX_COUNT = MAX_QUAD_COUNT * 6;
		static const size_t MAX_TEXTURE_SLOTS = 31;

		//for lines 
		static const size_t MAX_LINE_COUNT = 3000;
		static const size_t MAX_LINEVERTEX_COUNT = MAX_LINE_COUNT * 2;
		static const size_t MAX_LINEINDEX_COUNT = MAX_LINEVERTEX_COUNT;

		//for triangles
		static const size_t MAX_TRIANGLE_COUNT = 3000;
		static const size_t MAX_TRIANGLE_VERTEX_COUNT = MAX_TRIANGLE_COUNT * 3;
		static const size_t MAX_TRIANGLE_INDEX_COUNT = MAX_TRIANGLE_VERTEX_COUNT;

	};
	
	//Adds a quad to the batch to be rendererd 
	void AddQuad(const float& x, const float& y,const float& z, const std::string& textureName, const float& scalex = 1.0f,const float& scaley = 1.0f, const Mathf::Vector4& clr = { 1.f,1.f,1.f,1.f }, const std::array<Mathf::Vector2, 4>& texCoord = { Mathf::Vector2{0.f,0.f},Mathf::Vector2{1.0f,0.f},
		Mathf::Vector2{1.f,1.f},Mathf::Vector2{0.f,1.f} });

	//Adds a quad to the batch to be rendererd 
	void AddRotatedQuad(const glm::mat4& transform,const float& z, const std::string& textureName, const Mathf::Vector4& clr = { 1.f,1.f,1.f,1.f },const std::array<Mathf::Vector2, 4>& texCoord = { Mathf::Vector2{0.f,0.f},Mathf::Vector2{1.0f,0.f},
		Mathf::Vector2{1.f,1.f},Mathf::Vector2{0.f,1.f} });

	void AddRotateLine(const float& x0, const float& y0, const float& x1, const float& y1,const float& r = 1.f, const float& g = 1.f, const float& b = 1.f);

	void AddRotateTriangle(const float& x0, const float& y0, const float& x1, const float& y1, const float& x2, const float& y2, const float& r = 1.f, const float& g = 1.f, const float& b = 1.f);

	//Adds a quad to the batch to be rendered, no texture just mesh  
	void AddRotatedQuadMesh(const glm::mat4& transform, const float& z, const Mathf::Vector4& clr = { 1.f,1.f,1.f,1.f });

	//draws the entire batch 
	void Draw(const float& cameraHeight, const Mathf::vec3& cameraPosition) const;

	//draws the entire batch of quad meshes 
	void DrawQuadMesh(const float& cameraHeight, const Mathf::vec3& cameraPosition) const;

	//draws the entire batch of lines
	void DrawLine(const float& cameraHeight, const Mathf::vec3& cameraPosition) const;

	//draws the triangle
	void DrawTriangle(const float& cameraHeight, const Mathf::vec3& cameraPosition) const;

	//checks for overflow 
	void CheckOverFlow(const float& cameraHeight,const Mathf::vec3& cameraPosition);

	//flushing of the batch renderer 
	void Flush();

	//singleton 
	inline static BatchRenderer2D& GetInstance() { static BatchRenderer2D instance; return instance; } 

	BatchRenderer2D(const BatchRenderer2D& other) = delete;//delete copy constructor 
	~BatchRenderer2D() = default; //dstor
	BatchRenderer2D(); //cstor to initialize the batch 
	BatchRenderer2D& operator= (const BatchRenderer2D& other) = delete;//delete assignment operator 

	//ptrs to data 
	std::unique_ptr<VertexArray> m_vaoidQuad;
	std::unique_ptr<VertexBuffer> m_vboidQuad;
	std::unique_ptr<IndexBuffer> m_iboidQuad;
	std::unique_ptr<Shader> m_shaderidQuad;

	//for the lines
	std::unique_ptr<VertexArray>   m_vaoidLines;
	std::unique_ptr<VertexBuffer>  m_vboidLines;
	std::unique_ptr<IndexBuffer>   m_iboidLines;
	std::unique_ptr<Shader>        m_shaderidLines;
	
	//for the quad mesh 
	std::unique_ptr<VertexArray> m_vaoidQuadMesh;
	std::unique_ptr<VertexBuffer> m_vboidQuadMesh;
	std::unique_ptr<IndexBuffer> m_iboidQuadMesh;
	std::unique_ptr<Shader> m_shaderidQuadMesh;

	//for the triangle mesh 
	std::unique_ptr<VertexArray> m_vaoidTriangle;
	std::unique_ptr<VertexBuffer> m_vboidTriangle;
	std::unique_ptr<IndexBuffer> m_iboidTriangle;
	std::unique_ptr<Shader> m_shaderidTriangle;

private:
	uint32_t numOfQuadsDrawn;//number of quads drawn
	uint32_t numOfLinesDrawn;//number of lines drawn
	uint32_t numOfQuadMeshDrawn;//number of mesh quads drawn 
	uint32_t numOfTriangleMeshDrawn;//number of mesh triangles drawn 

};


#endif // ! BATCHRENDERER2D_H
