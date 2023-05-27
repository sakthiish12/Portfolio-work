#ifndef _EMITTER_H_
#define _EMITTER_H_

#include <vector>
#include <memory>
#include "../../../Math/Vector/Vector2.h"
#include "Particle.h"

//#include "ParticleSystem.h"

//1 partical system only to manage emitter
//emitter should be able to have different varition of classes for different effect,
    //which stores particle in particle pool
//Particle should only have 1 type

//Try blending with other modes, such as ADD, SUBTRACT, LIGHTEST, DARKEST, DIFFERENCE, EXCLUSION,or MULTIPLY.

/*
* Variance: + and - in addition to specified value
* TODO: THIS PROBABLY NEED A SEARLISATION for different kind of emitter
*/
//Data to be passed to particle*****************************************************************************
struct ParticleGenerationProperties
{
    //Maybe can do something like enum boolean like win32 STYLE
    using vec2 = Mathf::vec2;
    using vec3 = Mathf::vec3;

    //pTransform
    vec3 spawnPos{ 0.f, 0.f, 1.5f }, spawnPosVariance;
    vec2 scale = { 100.f, 100.f }, scaleVariance;
    vec2 startScale, endScale;
    int EntityToFollow = -1; // 0 and greater means to following another entity pos

    //pPhysic
    float speed = 1.f, speedVariance;
    vec2 vel = { 100.f, 0.f }, velVariance;

    //pLifespan
    float lifespan, lifespanVariance;

    //pColor
    glm::vec4 startColor = { 1.f, 1.f, 0.f, 1.f }, endColor;

    //pTexture
    bool use_texture = true;
    std::string textureId = "Tree3.png"; //texture ptr
};

struct EmitterData
{
    //prewarm???

    //eLifespan
    float lifespan = 20.f; //in seconds, left alive, ignore if alwaysActive is true
    bool alwaysActive = false; // If true: ignore lifespan and forever alive
   
    //pSpawning Type
    float emmisionRate = 1.f; //Num to fire when timer end
    float emitter_Timer = 0.f; //Seconds to next fire
    bool spawnTrigger = false; // If true: ignore emitter_Timer
    int NumOfSpawnPerTrigger = 1;
    
    //pInitial Position/methods
    bool spwanUsingEmitterPos = true; //if true, will use emiter position, else use spawnPosCenter and spawnPosVariance
    float spawnRotation = 0.f; // spawn and set velocity in the range, 0 to spawnRotation, in a circle, starting from current rotation. 


    //eMax particle
    int maxParticle = 20;
};

//using ParticleGenerationFnPtr = void (*) (Particle& p, ParticleGenerationProperties& pP);

//Add this as a static member variable to derived class to enable self registering
//template<typename T>
//struct emitterSelfRegistering 
//{
//    emitterSelfRegistering(std::string const& s);
//    
//};

struct Particle;
class ParticlePool;
struct Emitter
{
    std::string name {"EmitterBase"}; //name of derived class
    std::string IDname {"EmitterBase00"}; //name of serialised data // should be uniquely identifiable
    std::string path {}; //path to the serialised file
    ////TODO WIP
    ////fn ptr to modify generation
    //ParticleGenerationFnPtr _ParticleGenerationFnPtr = nullptr;

    using vec2 = Mathf::vec2;
    using vec3 = Mathf::vec3;
    enum EMITTER_STATE {
        DEAD = 0,
        ALIVE,
        DEAD_BUT_PARTICLE_ALIVE,
        FOREVER_ALIVE
    };
    unsigned int id;
    vec3 pos;

    //Normal Emitter Data============================================================================
    EmitterData _emitterData;
    ParticleGenerationProperties _newParticleProperties{};

    std::shared_ptr<ParticlePool> PP;

    //virtual std::shared_ptr<scriptBase> createCopy() = 0;

    //MUST CHECK IF THE COPY COPY WITH THE DATA AS SAME
    virtual std::shared_ptr<Emitter> createCopy() = 0;
    friend class emitterManager;
public:
    Emitter(std::string name = "EmitterBase");
    virtual ~Emitter() = default;
    virtual void update(float dt);
    virtual bool updateLife(float dt);
    int GetPoolSize();
    std::string getScriptName();
    std::string getPath();
    void changePath(std::string newPath);
    std::string getIDname();
};









//Sample data of other kind
 /* <emitter type = "burst">
    <angleRange min = "0" max = "360" / >
    <rotSpeed value = "1" randMin = "-1" randMax = "1" / >
    <startSpeed value = "300" randMin = "0" randMax = "1" / >
    <endSpeed value = "300" randMin = "0" randMax = "1" / >
    <startSize value = "20" randMin = "0" randMax = "1" / >
    <endSize value = " 100" randMin = "1" randMax = "1" / >
    <emitNumber value = "60" / >
    <emitVariance value = "80" randMin = "0" randMax = "1" / >
    <maxParticleLife value = "150" randMin = "0" randMax = "1" / >
    <textureRect x = "0" y = "0" w = "128" h = "128" / >
    <startColor r = "255" g = "100" b = "0" a = "255" / >
    <endColor r = "210" g = "210" b = "210" a = "0" / >
    <blendMode mode = "add" / >
    <lifetime value = "50" / >
    <vortexSensitive state = "true" / >
    < / emitter>*/

//#include "ParticleSystem.h"

//template<typename T>
//emitterSelfRegistering<T>::emitterSelfRegistering(std::string const& s)
//{
//    EmitterManager.getEmitterDatabase()->insert(std::make_pair(s, std::make_shared<T>()));
//}

#endif
