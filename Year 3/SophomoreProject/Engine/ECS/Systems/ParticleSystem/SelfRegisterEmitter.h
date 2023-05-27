#ifndef _SELFREGISTER_EMITTER_H_
#define _SELFREGISTER_EMITTER_H_

#include <string>
//for EmitterManager
#include "ParticleSystem.h"

//Add this as a static member variable to derived class to enable self registering
template<typename T>
struct emitterSelfRegistering 
{
    emitterSelfRegistering(std::string const& s);
    
};
 
template<typename T>
emitterSelfRegistering<T>::emitterSelfRegistering(std::string const& s)
{
    EmitterManager.getEmitterDatabase().insert(std::make_pair(s, std::make_shared<T>()));
}

#endif
