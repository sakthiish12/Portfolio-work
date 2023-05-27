// /******************************************************************************/
// /*!
// \file         PostProcessing.h
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         8th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function declarations for the post processing 
// \lines        
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef POST_PROCESSING_H
#define POST_PROCESSING_H

#include <iostream>
#include <Framebuffer.h>//framebuffer 


class PostProcessing {
public:

	PostProcessing();//ctor
	~PostProcessing() = default;//dtor 

	//getting the instnace of the class 
	inline static PostProcessing& GetInstance() {
		static PostProcessing instance; return instance;
	}

	//bind and unbind the pre processing 
	void bindPreProcess();
	void unBindPreProessing();

	//bind and unbind the post processing 
	void bindPostProcessing();
	void unBindPostProcessing();

	void drawPostProcessing();//draws the post processing 

	//stores the effects of the post processing 
	enum class Effects {
		effectNormal,
		effectGreyScale,
		effectInverse,
		effectInversion,
		effectShake,
		effectBlur
	};

	float m_AlphaValue = 1.0;//indicates the alpha value of the pre processing 

	Effects currentEffects = Effects::effectNormal;//the current effect for the post processing
	bool toggleEffects = false;//indicates that the toggle for change of effects 

	std::unique_ptr<Framebuffer> m_PreProcessingFrame;//stores the pre processing 
	std::unique_ptr<Framebuffer> m_PostProcessingFrame;//stores the post processing
	std::unique_ptr<Shader> m_PostProcessingShader;//shader of the post processing 

private:

	unsigned int m_VAOPost;//post processing vao 



};

#endif // !POST_PROCESSING_H
