#include "BasicTransformations.h"

namespace TH
{

	void RotationScript::Update(double dt)
	{
		if (enable) {
			//Rotate the rig
 			BE::Vec3f rigRotation = BE::ECS->GetComponent<BE::Transform>(id).GetEulerRotation();
			rigRotation.y += rateOfRotation * (float)dt;
			BE::ECS->GetComponent<BE::Transform>(id).SetEulerRotation(rigRotation);
		}

	}

}


RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	using namespace TH;

	registration::class_<RotationScript>(RegistrationNameCreator<RotationScript>())
		.constructor()
		.property("RateOfRotation", &RotationScript::rateOfRotation);

}