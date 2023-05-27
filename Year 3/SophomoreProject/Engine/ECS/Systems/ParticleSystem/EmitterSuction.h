#ifndef _EMITTER_SUCTION_H_
#define _EMITTER_SUCTION_H_

#include "Emitter.h"
#include "ParticlePool.h"


struct EmitterSuction: public Emitter
{
    bool suctionTriggered = false; //will fire off particle if true, auto reset to false afterward
    float targetAngle;
    float angleArc = 3.141592654f * 0.125f; //the arc range
    float suctionLimit = 250.f; 
public:
    EmitterSuction();
    virtual ~EmitterSuction() = default;
    void init();
    void update(float dt) override;
    void setSuctionTriggered(bool newState);
    void setSuctionTriggered(bool newState, float target_angle, float angle_arc, float radiusLen);
    std::shared_ptr<Emitter> createCopy() override;
};

#endif
