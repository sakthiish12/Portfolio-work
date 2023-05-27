
#include "Timers.h"

namespace TH
{
	using namespace BE;



}
RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	using namespace TH;
	registration::class_<Timer>(RegistrationNameCreator<Timer>())
		.constructor()
		.property("Timer Duration", &Timer::duration);
}
