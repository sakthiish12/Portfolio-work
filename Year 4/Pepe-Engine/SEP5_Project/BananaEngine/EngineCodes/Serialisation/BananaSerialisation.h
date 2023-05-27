#ifndef BANANA_SERIALISATION_H
#define BANANA_SERIALISATION_H
#include <Precompiled/pch.h>
#include <Components/ECSComponent.h>
#include <Systems/ECS/ECS.h>
#define DONT_USE_ARRAY_FOR_LEVELFILES
namespace BE
{
	namespace Serialisation
	{
		enum class MetaDataKey
		{
			HiddenFromEditor,
			Serialize,
			Copy,
			EntityFlag,
			EntityTag,
			IsLocal,
			AudioSource,
			Renderer,
		};
		void LoadEntities(const std::string& fileName);
		void SaveEntities(const std::string& output);
		const std::string filePathName = "filePath";
		const std::string serializeAssetMethodName = "Serialise";
		const std::string assetConstructMethodName = "Construct";
		rttr::variant LoadArimethicTypes(const rapidjson::Value& curr, const rttr::type& givenType);
		rttr::variant LoadStringValue(const rapidjson::Value& curr);
		void LoadObjectRecur(const rapidjson::Value& curr, const rttr::type& componentType, const BEID& id, const rttr::instance& inst);
		void VariantToValueMap(const rttr::variant& var, rapidjson::Value& val, rapidjson::Document& d);
		void SerializeToValueBase(const rttr::instance& inst, rapidjson::Value& v, rapidjson::Document& d);
	}
};
#endif // !1
