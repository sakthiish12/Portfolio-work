// /******************************************************************************/
// /*!
// \file         .cpp
// \project name Engine
// \author(s)    Sakthiish
// \date         10th Apr, 2022 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definition of the pause quit game script 
// \lines        
// 
// All content :copyright: 2022 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2022 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef _EMITTER_DIG_H_
#define _EMITTER_DIG_H_

#include "Emitter.h"

struct EmitterDig : public Emitter
{
    bool isTriggered = false; //will fire off particle if true, auto reset to false afterward
    //float targetAngle;
    //float angleArc = 3.141592654f * 0.125f; //the arc range
    //float suctionLimit = 70.f; 
    vec2 _movementVel = { 0.f, 0.f };
public:
    EmitterDig();
    virtual ~EmitterDig() = default;
    void init();
    void update(float dt) override;
    void setTriggered(bool newState, vec2& movementVel, vec3& particleSpawnAt);
    std::shared_ptr<Emitter> createCopy() override;
};

#endif
