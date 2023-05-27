#ifndef _PARTICLE_SYSTEM_
#define _PARTICLE_SYSTEM_

#include <vector>
#include "../../system.h"


//references / learning sources / possible
//https://nintervik.github.io/2D-Particle-System/#231-the-emitters
//http://gameprogrammingpatterns.com/object-pool.html
//Cherno: https://www.youtube.com/watch?v=GK0jHlv3e3w
//possible: http://buildnewgames.com/particle-systems/
//possible, but cost $$: https://www.cppstories.com/2014/04/flexible-particle-system-container/


//structure
// Emitter inside have ParticlePools
// ParticlePool manages all the particles for that emitter

//emitter could be a component
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct Emitter;
class emitterManager
{
	//for emitter manager, the string name will be the IDname (of emitter base) because it should be unique
	std::map <std::string, std::shared_ptr< Emitter > > allEmitterContainer{};
	std::vector<std::string> _EmitterToBeRemoved{};
public:
	void addEmitter(std::shared_ptr<Emitter> emi);

	//template <typename DerivedT>
	//void addEmitter();

	void deleteEmitter(std::shared_ptr<Emitter> emi);
	void deleteEmitter(std::string emi);
	static emitterManager& getInstance();

	std::shared_ptr<Emitter> createFromEmitterDatabase(std::string name);

	//get a specific script pointer
	std::shared_ptr<Emitter> getEmitter(std::string name);
	std::map<std::string, std::shared_ptr<Emitter> >& getEmitterDatabase();
};
#define EmitterManager ::emitterManager::getInstance()

class ParticleSystem : public systemBase
{
public:
	ParticleSystem(bool requireComponentMatch = false);
	~ParticleSystem() = default;

	void init();
	void update(float dt);
	void destoryEmitter();
	void destoryAllEmitter();

	void ParticleTesting();
};

extern std::shared_ptr<ParticleSystem> particleSystemPtr;
#define particleSystem particleSystemPtr

#endif
