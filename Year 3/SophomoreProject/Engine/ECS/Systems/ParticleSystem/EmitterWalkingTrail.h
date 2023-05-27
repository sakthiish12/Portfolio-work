#ifndef _EMITTER_WALKING_TRAIL_H_
#define _EMITTER_WALKING_TRAIL_H_

#include "Emitter.h"

struct EmitterWalkingTrail: public Emitter
{
    bool isTriggered = false; //will fire off particle if true, auto reset to false afterward
    //float targetAngle;
    //float angleArc = 3.141592654f * 0.125f; //the arc range
    //float suctionLimit = 70.f; 
    vec2 _movementVel = { 0.f, 0.f };
public:
    EmitterWalkingTrail();
    virtual ~EmitterWalkingTrail() = default;
    void init();
    void update(float dt) override;
    void setTriggered(bool newState, vec2& movementVel, vec3& particleSpawnAt);
    std::shared_ptr<Emitter> createCopy() override;
};

#endif
