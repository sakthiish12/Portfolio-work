#include "pch.h"
#include <Serialisation/BananaSerialisation.h>

RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	registration::class_<AudioListener>(RegistrationNameCreator<AudioListener>())
		.constructor();
}