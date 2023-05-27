#include <pch.h>
#include <Maths/Quaternion.h>
#include <Serialisation/BananaSerialisation.h>
const bool BE::serialiseQuat = true;
RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE::Serialisation;
	registration::class_<BE::Quatf>(BE::RegistrationNameCreator<BE::Quatf>())
	.constructor()
	(
	)
	.property("i", &BE::Quatf::i)
	(
		metadata(MetaDataKey::Serialize, true),
		metadata(MetaDataKey::Copy, true)
	)
	.property("j", &BE::Quatf::j)
	(
		metadata(MetaDataKey::Serialize, true),
		metadata(MetaDataKey::Copy, true)
	)
	.property("k", &BE::Quatf::k)
	(
		metadata(MetaDataKey::Serialize, true),
		metadata(MetaDataKey::Copy, true)
	)
	.property("r", &BE::Quatf::r)
	(
		metadata(MetaDataKey::Serialize, true),
		metadata(MetaDataKey::Copy, true)
	);
}