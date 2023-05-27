#ifndef _EMITTER_TELEPORTING_TRAIL_H_
#define _EMITTER_TELEPORTING_TRAIL_H_

#include "Emitter.h"

struct EmitterTeleporterTrail : public Emitter
{
    bool isTriggered = false; //will fire off particle if true, auto reset to false afterward
    vec2 _movementVel = { 0.f, 0.f };
public:
    EmitterTeleporterTrail();
    virtual ~EmitterTeleporterTrail() = default;
    void init();
    void update(float dt) override;
    void setTriggered(bool newState, vec2& movementVel, vec3& particleSpawnAt);
    std::shared_ptr<Emitter> createCopy() override;
};

#endif
