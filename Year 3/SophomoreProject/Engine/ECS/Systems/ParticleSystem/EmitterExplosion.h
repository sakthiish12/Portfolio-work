#ifndef _EMITTER_EXPLOSION_H_
#define _EMITTER_EXPLOSION_H_

#include "Emitter.h"

struct EmitterExplosion: public Emitter
{
    bool isTriggered = false; //will fire off particle if true, auto reset to false afterward
    float targetAngle;
    float angleArc = 3.141592654f * 0.125f; //the arc range
    float suctionLimit = 70.f; 
public:
    EmitterExplosion();
    virtual ~EmitterExplosion() = default;
    void init();
    void update(float dt) override;
    void setTriggered(bool newState);
    std::shared_ptr<Emitter> createCopy() override;
};

#endif
