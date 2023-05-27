#include <pch.h>
#include <Maths/Vector3D.h>
#include <Serialisation/BananaSerialisation.h>
const bool BE::serialiseVec = true;
RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE::Serialisation;
	registration::class_<BE::Vec3f>(BE::RegistrationNameCreator<BE::Vec3f>())
	.constructor()
	(
	)
	.property("x", &BE::Vec3f::x)
	(
		metadata(MetaDataKey::Serialize,true),
		metadata(MetaDataKey::Copy,true)
	)
	.property("y", &BE::Vec3f::y)
	(
		metadata(MetaDataKey::Serialize, true),
		metadata(MetaDataKey::Copy, true)
	)
	.property("z", &BE::Vec3f::z)
	(
		metadata(MetaDataKey::Serialize, true),
		metadata(MetaDataKey::Copy, true)
	);
	registration::class_<glm::vec3>(BE::RegistrationNameCreator<glm::vec3>())
	.constructor()
	(
	)
	.property("x", &glm::vec3::x)
	(
		metadata(MetaDataKey::Serialize, true),
		metadata(MetaDataKey::Copy, true)
	)
	.property("y", &glm::vec3::y)
	(
		metadata(MetaDataKey::Serialize, true),
		metadata(MetaDataKey::Copy, true)
	)
	.property("z", &glm::vec3::z)
	(
		metadata(MetaDataKey::Serialize, true),
		metadata(MetaDataKey::Copy, true)
	);
}